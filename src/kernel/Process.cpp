#include "kernel/Process.hpp"

//Variables definitions 
uint8_t Process::PID{0};

//Function definitions 

void Process::init(TaskFunction task, uint8_t priority)
{
    m_task = task;

    m_priority = priority;

    m_pid = ++PID;

    m_state = State::READY;

    stackInit();
}

void Process::stackInit()
{
    m_sp = &m_stack[STACK_SIZE - 1];

    uint16_t funcPtr = reinterpret_cast<uint16_t>(m_task);              //change m_task type from 1 to 2 bytes bcs asm needs it 

    // store return address 
    // Harware loads PC: PC Low then PC High 
    *m_sp-- = (uint8_t)(funcPtr & 0xFF);         // PC Low    (PC is program counter)
    *m_sp-- = (uint8_t)((funcPtr >> 8) & 0xFF);  // PC High

    // Registers that switchContextASM will take out 'pop'
    *m_sp-- = 0x00; // R0
    *m_sp-- = 0x80; // SREG 0x80 has interrupts 
    *m_sp-- = 0x00; // R1 C++

    for (uint8_t i = 2; i <= 31; ++i)
    {
        *m_sp-- = 0x00;
    }
}

//Constructor definitions
Process::Process(TaskFunction task, uint8_t priority)
    : m_task(task)
    , m_state(State::READY)
    , m_priority(priority)
{
  m_pid = ++PID;

  Process::stackInit();
}
