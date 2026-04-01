#include <avr/io.h>
#include <avr/interrupt.h>

#include "kernel/Kernel.hpp"

extern "C" void switchContextASM(Process* current, Process* next);


//------------------------------------------------
// Context Switch
//------------------------------------------------

void Kernel::switchContext(Process* next)
{
    Process* current = m_currentProcess;

    m_currentProcess = next;

    switchContextASM(current, next);
}


//------------------------------------------------
// Timer0 init (1ms tick)
//------------------------------------------------

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
    // 250000 / 1000 = 250 ticks
    OCR0A = 249;

    // Enable interrupt
    TIMSK0 |= (1 << OCIE0A);

    sei();
}


//------------------------------------------------
// Timer interrupt
//------------------------------------------------

ISR(TIMER0_COMPA_vect)
{
    Kernel::getInstance().scheduler();
}


//------------------------------------------------
// Scheduler (test with 2 processes)
//------------------------------------------------

void Kernel::scheduler()
{
    if (m_processCount < 2) return;

    if (m_currentProcess == &m_PCB[0])
    {
        switchContext(&m_PCB[1]);
    }
    else
    {
        switchContext(&m_PCB[0]);
    }
}


//------------------------------------------------
// Task creation
//------------------------------------------------

void Kernel::createTask(void (*taskFunction)(), uint8_t priority)
{
    if (m_processCount >= MAX_PROCESSES)
        return;

    m_PCB[m_processCount].init(taskFunction, priority);

    m_processCount++;
}


//------------------------------------------------
// Start Kernel
//------------------------------------------------

void Kernel::start()
{
    if (m_processCount == 0)
        return;

    m_currentProcess = &m_PCB[0];

    initTimer0();

    sei();

    // comenzar ejecutando el primer proceso
    switchContext(m_currentProcess);
}


//------------------------------------------------
// Singleton
//------------------------------------------------

Kernel Kernel::instance;

Kernel& Kernel::getInstance()
{
    return instance;
}


//------------------------------------------------
// Constructor
//------------------------------------------------

Kernel::Kernel()
{
    m_currentProcess = nullptr;
    m_processCount = 0;
}
