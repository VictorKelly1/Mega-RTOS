#include <avr/io.h>
#include <avr/interrupt.h>

#include "kernel/Kernel.hpp" 

//Function definitions
extern "C" void switchContextASM(Process* current, Process* next);
void Kernel::switchContext(Process* next){

    Process* current = m_currentProcess;

    m_currentProcess = next;

    switchContextASM(current, next);
}

void Kernel::initTimer0(){
    cli();                                                                          //Disable interruptions 

    TCCR0A = 0;                                                                     //Clean timer's registers
    TCCR0B = 0;                                             

    TCCR0A |= (1 << WGM01);                                                         //Register clear time comparing match mode (resete register when comparation is true with OCR0A)
    TCCR0B |= (1 << CS01) | (1 << CS00);                                            //Prescaler at 64 for calculate 1ms whit 250,000Hz 
    //(16 Mhz / 64 = 250000 Hz) / 1000 = 250 
    OCR0A = 249;                                                                    //249 because start in 0 

    TIMSK0 |= (1 << OCIE0A);

    sei();
}
ISR(TIMER0_COMPA_vect){                                                             //Call sched 1 time per ms 
    Kernel::getInstance().scheduler();
}

void Kernel::scheduler(){
    if (m_currentProcess == m_PCB[0])
    {
        switchContext(m_PCB[1]);
    }
    else
    {
        switchContext(m_PCB[0]);
    }
}

void Kernel::createTask(void (*taskFunction)(), uint8_t priority){
    if(m_processCount >= MAX_PROCESSES) return; 

    //Create and add a process to list 
    Process* newProcess = new Process(taskFunction, priority);

    m_PCB[m_processCount] = newProcess;

    ++m_processCount;
}

void Kernel::start(){
    if(m_processCount <= 0) return;

    m_currentProcess = m_PCB[0];

    sei();                                                                              //enable interruptions
    initTimer0();
}

//Singlenton instance 
Kernel& Kernel::getInstance(){
    static Kernel instance{};
    return instance;
}

//Constructor definitions
Kernel::Kernel(){
    m_currentProcess = nullptr;
    m_processCount = 0;
    
    for (uint8_t index {}; index < MAX_PROCESSES; ++index){
        m_PCB[index] = nullptr;
    }
}
