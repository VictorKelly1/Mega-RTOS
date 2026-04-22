/*
 * @file Kernel.hpp
 * @author Victor Kelly 
 * @brief Core real-time scheduler and kernel management for megaRTOS.
 * This class implements the Singleton pattern to manage task scheduling, 
 * time-based events (ISR)(delays), and process synchronization primitives.
*/

#ifndef KERNEL_HPP
#define KERNEL_HPP

#include <avr/io.h>
#include <stdint.h>

#include "Process.hpp"
#include "kernel/Config.hpp"

/*
 * @class Kernel
 * @brief The central controller of the Operating System.
 * Handles the Round-Robin preemptive scheduling, task creation, and 
 * the transition between user tasks and the system idle task.
*/
    /*
     * @brief Returns the global instance of the Kernel.
     * @return Kernel& Reference to the singleton instance.
     */

    /*
     * @brief Deleted copy constructor and assignment operator to enforce Singleton pattern.
*/

extern "C" void TIMER0_COMPA_vect(void) __attribute__ ((signal));

class Kernel final
{
public:
    static Kernel& getInstance();                   //Singleton aplied 
    
    Kernel(const Kernel&) = delete;
    Kernel& operator=(const Kernel&) = delete;

    //Func declarations
    void createTask(void (*taskFunction)(), uint8_t priority); 
    void start();
    //void initTimer0();// 

    //static void idleTask();// 
    //CPU cicles functions 
    //void updateSleepers();// 
    void delay(uint16_t ms);

    //Getters / setters
    Process* getCurrentProcess() const { return m_currentProcess; } 
    void setCurrentProcess(Process* newProcess) { m_currentProcess = newProcess; } 

    uint8_t getProcessCount() const { return m_processCount; }
private:         //Not usable functions for user 
    static void idleTask();

    void updateSleepers();
    void scheduler();
    void initTimer0();

    //I/O functions
    void blockCurrentProcess();
    void wakeProcess(Process* process);

    friend void TIMER0_COMPA_vect(void);
    friend class UART; 
    friend class Mutex;
private:

    static Kernel instance;

    Kernel();
    ~Kernel() = default;

private:                                                //Members 

    static constexpr uint8_t MAX_PCBS { MAX_PROCESSES };

    Process m_PCB[MAX_PCBS];

    Process* m_currentProcess{nullptr};

    Process*m_sleepingHead { nullptr };

    uint8_t m_currentIndex {0};

    uint8_t m_processCount{0};
};

/*
 * @brief External assembly routine for low-level context switching.
 * @param currentSP Address of the current process stack pointer variable.
 * @param nextSP Address of the next process stack pointer variable.
*/

extern "C" void switchContextASM(uint8_t* currentSP, uint8_t* nextSP);

#endif
