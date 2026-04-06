#ifndef KERNEL_HPP
#define KERNEL_HPP

#include <stdint.h>
#include "Process.hpp"

class Kernel final
{
public:
    // Singleton
    static Kernel& getInstance();

    Kernel(const Kernel&) = delete;
    Kernel& operator=(const Kernel&) = delete;

    // API
    void createTask(void (*taskFunction)(), uint8_t priority);
    void scheduler();
    void switchContext(Process* next);
    void start();
    void initTimer0();

    // Getters / setters
    Process* getCurrentProcess() const { return m_currentProcess; }
    void setCurrentProcess(Process* newProcess) { m_currentProcess = newProcess; }

    uint8_t getProcessCount() const { return m_processCount; }

private:

    static Kernel instance;     

    Kernel();
    ~Kernel() = default;

private:

    static constexpr uint8_t MAX_PROCESSES{6};

    Process m_PCB[MAX_PROCESSES];

    Process* m_currentProcess{nullptr};

    uint8_t m_currentIndex {0};

    uint8_t m_processCount{0};
};

extern "C" void switchContextASM(uint8_t* currentSP, uint8_t* nextSP);

#endif
