#include "kernel/Process.hpp"

//Variables definitions 
uint8_t Process::PID{0};

//Function definitions 

void Process::init(TaskFunction task, uint8_t priority)
{
    m_task = task;

    m_priority = priority;

    m_pid = PID++;

    m_state = State::READY;

    stackInit();
}

void Process::stackInit(){

  m_sp = &m_stack[STACK_SIZE - 1];

  uint16_t funcPtr = reinterpret_cast<uint16_t>(m_task); //Pointer to the task

  *m_sp-- = (uint8_t)( funcPtr & 0xFF);                  //Write in m_sp adress, then decrement the adress and extract low bit  
  *m_sp-- = (uint8_t)((funcPtr >> 8) & 0xFF);             

  *m_sp-- = 0x80;                                        //Activate interruptioins 

  for (uint8_t index {}; index < 32; ++index){           
        *m_sp-- = 0x00;
  }

  ++m_sp; 
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
