/*
 * @file Kernel.cpp
 * @brief Implementation of the megaRTOS Kernel logic.
 * Includes the preemptive scheduler, the delta queue for efficient delays,
 * and hardware timer configuration for the system tick.
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "kernel/Config.hpp"
#include "kernel/CriticalSection.hpp"
#include "kernel/Kernel.hpp"
#include "kernel/Process.hpp"

//Every process needs aprox 150 bytes 
//The ATmega328P only has 2048 bytes (2KB). 
//The TROS need space for global variables, stacks, objects atc. use more than 10 processes is dangerus 
static_assert(MAX_PROCESSES <= 8, "ERROR: MAX_PROCESSES is too much, the max is 8. It will overflow the RAM of the ATmega328P.");

//Function definitions
extern "C" void switchContextASM(uint8_t* current, uint8_t* next); 

/**
 * @brief Configures Timer0 to generate the System Tick.
 * Uses CTC (Clear Timer on Compare) mode with a prescaler of 64 for calculate 1ms with 250000Hz.
 * The frequency is dynamically calculated based on F_CPU and SYS_TICK_MS.
 *
 * @brief Timer0 ISR (Interrupt Service Routine).
 * Executed every SYS_TICK_MS. It updates the timing for blocked processes
 * and triggers the preemptive scheduler.
*/
void Kernel::initTimer0()               //SYS_TICK_MS and Frecuency is in Config.hpp 
{
    cli();
    //interrupt SYS_TICK_MS, Frecuency = 1000 / SYS_TICK_MS, for 1ms is 1000Hz.
    //comun prescaler 64 for 16MHz y 8MHz
    const uint32_t prescaler = 64;
    const uint32_t targetFreq = 1000 / SYS_TICK_MS;
    
    // Cálculo automático del valor de comparación
    // (F_CPU / (64 * 1000)) - 1
    constexpr uint8_t ocrValue = (uint8_t)((F_CPU / (prescaler * targetFreq)) - 1);

    TCCR0A = (1 << WGM01);              // Mode Clear on Compare with 0CR0A 
    TCCR0B = (1 << CS01) | (1 << CS00); // Prescaler 64 to count 1ms with 250000Hz and not with 16MHz
    OCR0A = ocrValue;                   // Calculated value 

    TIMSK0 |= (1 << OCIE0A); 
    
    sei();
}
/*
 * @brief Timer0 ISR (Interrupt Service Routine).
 * Executed every SYS_TICK_MS. It updates the timing for blocked processes
 * and triggers the preemptive scheduler.
*/ 
ISR(TIMER0_COMPA_vect)
{
    Kernel::getInstance().updateSleepers();
    Kernel::getInstance().scheduler();
}

/*
 * @brief Updates the sleep timers using a Delta Queue approach.
 * Instead of decrementing every task's counter, only the head of the list 
 * is updated, significantly reducing CPU overhead during each tick.
*/
void Kernel::updateSleepers() {
    if (m_sleepingHead == nullptr) return;

    //We rest 1 tick to the queue head
    m_sleepingHead->setDelta(m_sleepingHead->getDelta() - 1);

    // If the time of the first process in queue left, we wake it up
    // we use a while loop for wake up serveral processes 
    while (m_sleepingHead != nullptr && m_sleepingHead->getDelta() == 0) {
        Process* p = m_sleepingHead;
        
        p->setState(Process::State::READY); 
        
        //put it out of the delta queue 
        m_sleepingHead = p->getNextSleeper();
        p->setNextSleeper(nullptr); 
    }
}

/*
 * @brief Blocks the current process for a specific number of ms.
 * The task is inserted into a sorted Delta Queue. This is a non-busy wait;
 * the task yields the CPU and stays in BLOCKED state until the time expires.
 * @param ms Milliseconds to wait.
*/
void Kernel::delay(uint16_t ms) {
    if (ms == 0) return;

     

    uint16_t remaining = ms / SYS_TICK_MS;

    {//Critical CriticalSection
        CriticalSection active;

        Process* current = m_sleepingHead;
        Process* prev = nullptr;

        //find the correct position in queue 
        while (current != nullptr && remaining >= current->getDelta()) {
            remaining -= current->getDelta();
            prev = current;
            current = current->getNextSleeper();
        }

        //Configuration of the next process that is gooing to sleep 
        m_currentProcess->setDelta(remaining);
        m_currentProcess->setNextSleeper(current);
        m_currentProcess->setState(Process::State::BLOCKED);

        //Put it on the linked list 
        if (prev == nullptr) {
            m_sleepingHead = m_currentProcess; // Es el primero en despertar
        } else {
            prev->setNextSleeper(m_currentProcess);
        }

        //fix the time of the next one
        if (current != nullptr) {
            current->setDelta(current->getDelta() - remaining);
        }
    }//End of critical CriticalSection object active destroyed 

    scheduler(); // switch context with "Yelding" 
}

/*
 * @brief Round-Robin Preemptive Scheduler.
 * Selects the next process in the READY state to be executed.
 * If a context switch is required, it calls the assembly routine to swap stacks.
 * @note If no user tasks are ready, the system always finds the Idle Task.
*/
void Kernel::scheduler()
{
    if (m_processCount < 1) return; 

    // Si el proceso actual no está bloqueado (por delay), lo pasamos a READY
    if (m_currentProcess->getState() == Process::State::RUNNING)
    {
        m_currentProcess->setState(Process::State::READY);
    }

    uint8_t prevIndex = m_currentIndex;
    uint8_t nextIndex = prevIndex;

    // Buscamos el siguiente proceso READY (siempre habrá al menos uno: la Idle Task)
    for (uint8_t index { 0 }; index < m_processCount; ++index)
    {
        uint8_t candidate = (prevIndex + index + 1) % m_processCount;
        
        if (m_PCB[candidate].getState() == Process::State::READY)
        {
            nextIndex = candidate;
            break; 
        }
    }

    // Si el siguiente es distinto al actual, cambiamos de contexto
    if (nextIndex != prevIndex)
    {
        m_currentIndex = nextIndex;
        Process* nextProcess = &m_PCB[m_currentIndex];
        m_currentProcess = nextProcess;
        
        m_currentProcess->setState(Process::State::RUNNING);

        switchContextASM(
            (uint8_t*)m_PCB[prevIndex].getSPaddress(),
            (uint8_t*)nextProcess->getSPaddress()
        );
    }
    else 
    {
        // Si solo el actual está READY, sigue corriendo
        m_currentProcess->setState(Process::State::RUNNING);
    }
}

/*
 * @brief Instantiates a new task in the PCB array.
 * @param taskFunction Entry point of the task.
 * @param priority Task priority (Reserved for future multi-level scheduling).
*/
void Kernel::createTask(void (*taskFunction)(), uint8_t priority)
{
    CriticalSection active;

    if (m_processCount >= MAX_PROCESSES)
        return;

    m_PCB[m_processCount].init(taskFunction, priority);

    m_processCount++;
}

/*
 * @brief Low-power Idle Task.
 * Executed when no other tasks are ready. It puts the CPU into IDLE sleep mode
 * to save energy, waking up only for hardware interrupts.
*/
void Kernel::idleTask() {
    set_sleep_mode(SLEEP_MODE_IDLE);
    while (true) {
        sleep_enable();
        sei();
        sleep_cpu();
        sleep_disable();
    }
}

/*
 * @brief Bootstraps the OS.
 * Creates the mandatory Idle Task, initializes the system timer, 
 * and performs the first context switch to jump into the first task.
*/
void Kernel::start() {

    createTask(idleTask, 0);

    if (m_processCount == 0) return;

    m_currentProcess = &m_PCB[0]; 
    // Init the Timer0 but SEI will activate when  
    // we restore the SREG of the first task that has 0x80 
    initTimer0(); 

    // Jump to first task 
    // Give m_sp to the first task to the switch context in ASM de carga
    uint8_t* dummySP; 
    switchContextASM((uint8_t*)&dummySP, (uint8_t*)m_currentProcess->getSPaddress());

}

/*
 * @brief Moves the current task to a WAITING_IO state.
 * Used by drivers (like UART) to suspend a task until data is available,
 * preventing polling and saving CPU cycles.
*/
void Kernel::blockCurrentProcess() {
    m_currentProcess->setState(Process::State::WAITING_IO);
    scheduler();
}

void Kernel::wakeProcess(Process* p) {
    if (p != nullptr) {
        p->setState(Process::State::READY);
    }
}

Kernel Kernel::instance;

Kernel& Kernel::getInstance()
{

    return instance;
}

Kernel::Kernel()
{
    m_currentProcess = nullptr;
    m_processCount = 0;
    m_currentIndex = 0; 
    m_sleepingHead = nullptr;
}
