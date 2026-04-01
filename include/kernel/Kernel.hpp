#ifndef KERNEL_HPP
#define KERNEL_HPP

#include <stdint.h>
#include "Process.hpp"

class Kernel final
{
public:                                                                     //public interface and singleton aplied
    static Kernel& getInstance();

    Kernel(const Kernel&) = delete;
    Kernel& operator=(const Kernel&) = delete;

    //Func definitions
    void createTask(void (*taskFunction)(), uint8_t priority);
    void scheduler();
    void switchContext(Process* next);
    void start();
    void initTimer0();

    //Getters and Setters
    Process* getCurrentProcess() const { return m_currentProcess; }
    void setCurrentProcess(Process* newProcess) { m_currentProcess = newProcess; }

    uint8_t getProcessCount() const { return m_processCount; }

private: 
    
    Kernel();

    ~Kernel() = default;

private:
    //list of processes 
    static constexpr uint8_t MAX_PROCESSES { 2 }; 
    Process* m_PCB[MAX_PROCESSES];

    //Ptr to current process in CPU
    Process* m_currentProcess {nullptr};

    uint8_t m_processCount {};
};

extern "C" void switchContextASM(Process* current, Process* next);

#endif 
