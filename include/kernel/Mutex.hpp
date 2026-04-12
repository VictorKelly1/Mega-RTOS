/*
 * @file Mutex.hpp
 * @author Victor Kelly 
 * @brief Mutual Exclusion primitive for task synchronization.
 * This class prevents race conditions by ensuring that only one task can 
 * access a shared resource at a time. It manages a waiting list to block 
 * tasks, preventing wasted CPU cycles during contention.
*/

#ifndef MUTEX_HPP
#define MUTEX_HPP

#include "kernel/Kernel.hpp"
#include "kernel/Process.hpp"
#include "kernel/CriticalSection.hpp"

/*
 * @class Mutex
 * @brief Synchronization primitive for protecting shared resources.
 * Implements a "Lock/Unlock" mechanism. If a resource is already owned, 
 * the requesting task is moved to a WAITING_MUTEX state and added to 
 * a linked list until the resource is released.
*/
class Mutex final {
private:
    Process* m_owner;                   //Ptr to process who has the resource 
    Process* m_waitListHead;            //Head of the Linked List(All the process that are waiting for a resource)

public:
    //Constructor 
    Mutex()
      :  m_owner(nullptr)
      ,  m_waitListHead(nullptr)
    {}

    void lock(){
        while(true){
            {//Block critical Section 
                CriticalSection active;
                
                if(m_owner == nullptr){                                         //If there is not a owner in mutex 
                    m_owner = Kernel::getInstance().getCurrentProcess();        //the current process us the new owner 
                    return;
                }
                //if the mutex has owner, put the process in queue 
                Process* current = Kernel::getInstance().getCurrentProcess();
                current->setState(Process::State::WAITING_MUTEX);

                current->setNextSleeper(m_waitListHead);
                m_waitListHead = current; 
            }//Critical section object destroyed
            Kernel::getInstance().scheduler();
        }
    }

    void unlock(){
        CriticalSection active;

        if(m_owner == Kernel::getInstance().getCurrentProcess()){
            m_owner = nullptr;

            if(m_waitListHead != nullptr){
                //wake up the next process in queue
                Process* toWake = m_waitListHead;
                m_waitListHead = toWake->getNextSleeper();

                toWake->setNextSleeper(nullptr);
                toWake->setState(Process::State::READY);
            }
        }
    }
};

#endif 
