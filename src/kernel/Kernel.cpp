#include <avr/io.h>
#include <avr/interrupt.h>

#include "kernel/Config.hpp"
#include "kernel/Kernel.hpp"
#include "kernel/Process.hpp"

//Every process needs aprox 150 bytes 
//The ATmega328P only has 2048 bytes (2KB). 
//The TROS need space for global variables, stacks, objects atc. use more than 10 processes is dangerus 
static_assert(MAX_PROCESSES <= 10, "ERROR: MAX_PROCESSES is too much. It will overflow the RAM of the ATmega328P.");

extern "C" void switchContextASM(uint8_t* current, uint8_t* next); 

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
ISR(TIMER0_COMPA_vect)
{
    Kernel::getInstance().scheduler();
}

void Kernel::scheduler()
{
    if (m_processCount < 2) return;                                 //we need at least 2 processes for rotation

    m_currentProcess->setState(Process::State::READY);

    uint8_t prevIndex = m_currentIndex;
    uint8_t nextIndex = prevIndex;

    for (uint8_t index { 1 }; index <= m_processCount; ++index)
    {
        uint8_t candidate = (prevIndex + index) % m_processCount;  
        
        if (m_PCB[candidate].getState() == Process::State::READY)
        {
            nextIndex = candidate;
            break; 
        }
    }

    if (nextIndex != prevIndex)                                     //if the next process is diferent from the current one
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
        m_currentProcess->setState(Process::State::RUNNING);        //if there is not a new process the current one keeps running 
    }
}

void Kernel::createTask(void (*taskFunction)(), uint8_t priority)
{
    if (m_processCount >= MAX_PROCESSES)
        return;

    m_PCB[m_processCount].init(taskFunction, priority);

    m_processCount++;

}

void Kernel::start() {
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

Kernel Kernel::instance;

Kernel& Kernel::getInstance()
{

    return instance;
}

Kernel::Kernel()
{
    m_currentProcess = nullptr;
    m_processCount = 0;
}
