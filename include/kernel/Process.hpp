#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <stddef.h>
#include <stdint.h>

class Process final {
public: 
    enum class State : uint8_t { 
        READY, 
        RUNNING, 
        BLOCKED 
    };

private:
  using TaskFunction = void (*)();  //function Ptr 

  static constexpr uint16_t STACK_SIZE{128};

  static uint8_t PID;

private:
  TaskFunction m_task;

  uint8_t m_stack[STACK_SIZE]; // static size of 128 words
  uint8_t* m_sp;

  State m_state;

  uint8_t m_pid;
  uint8_t m_priority;

  uint16_t m_deltaTicks { 0 };        //Relative time to previous process 
  Process* m_nextSleeper { nullptr };       //Ptr to linked list delta queue 

public:
  // Constructor
  Process() = default;

  Process(TaskFunction task, uint8_t priority);

  //Member Fucntions
  void init(TaskFunction task, uint8_t priority);
  void stackInit();

  //Getters and Setters
  uint8_t* getSP() const { return m_sp; }
  uint8_t** getSPaddress() {return &m_sp; }             //ptr to ptr
  void setSP(uint8_t* sp) { m_sp = sp; }

  uint8_t getPID() const { return m_pid; }
  uint8_t getPriority() const { return m_priority; }

  void setState(State state) { m_state = state; }
  State getState() const { return m_state; }

  void setDelta(uint16_t delta) { m_deltaTicks = delta; }
  uint16_t getDelta() const { return m_deltaTicks; }

  void setNextSleeper(Process* next) { m_nextSleeper = next; }
  Process* getNextSleeper() { return m_nextSleeper; }
};

#endif
