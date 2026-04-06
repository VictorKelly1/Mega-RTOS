#include <avr/io.h>
#include <avr/interrupt.h>

#include "kernel/Kernel.hpp"
#include "kernel/Process.hpp"

extern "C" void switchContextASM(uint8_t* current, uint8_t* next);

void Kernel::initTimer0()
{
    cli();

    TCCR0A = 0;
    TCCR0B = 0;

    // CTC Mode
    TCCR0A |= (1 << WGM01);

    // Prescaler 64
    TCCR0B |= (1 << CS01) | (1 << CS00);

    // 16MHz / 64 = 250000 Hz
    // 250000 / 1000 = 250 ticks                //This is the quantum time for Round Robin scheduler 
    OCR0A = 249;

    // Enable interrupt
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
    //m_currentProcess->setState(Process::State::RUNNING);
    
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
