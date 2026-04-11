/**
 * @file Process.cpp
 * @brief Implementation of the Process class for task context management.
 * This file handles the lifecycle of a task's control block, specifically 
 * the simulation of the AVR stack frame required for the first context switch.
*/

#include "kernel/Process.hpp"

/* @brief Global Process ID counter. 
 * Incremented every time a new process is instantiated to ensure unique identification.
*/
uint8_t Process::PID{0};

//Function definitions 

/*
 * @brief Manual initialization of a process instance.
 *   Used primarily if the process is part of a pre-allocated pool where the 
 *   constructor isn't called directly during runtime. (Used when a task is created directly in proces control block) 
 * @param task Function pointer to the task code.
 * @param priority Execution priority for the scheduler.
*/

void Process::init(TaskFunction task, uint8_t priority)
{
    m_task = task;

    m_priority = priority;

    m_pid = ++PID;

    m_state = State::READY;

    stackInit();
}

/*
 * @brief Prepares the process stack to simulate an interrupted state.
 *  It manually pushes the initial task address (Program Counter m_sp(stack pointer))
 *  and a default set of registers onto the stack. When switchContextASM executes 
 *  its 'pop' sequence for the first time, it will return into the task function.
 * @note The SREG is initialized to 0x80 to ensure global interrupts (I-bit) 
 *  are enabled when the task starts.
 * @note R1 is initialized to 0x00 as per the avr-gcc ABI convention (__zero_reg__).
*/

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

/*
 * @brief Constructor for the Process class.
 *  Allocates a unique PID and prepares the task's execution context by 
 *  initializing its private stack.
 * @param task The function to be executed as a task.
 * @param priority Task priority level.
*/

Process::Process(TaskFunction task, uint8_t priority)
    : m_task(task)
    , m_state(State::READY)
    , m_priority(priority)
{
  m_pid = ++PID;

  Process::stackInit();
}
