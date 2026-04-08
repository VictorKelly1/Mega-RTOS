#ifndef CRITICAL_SECTION_HPP
#define CRITICAL_SECTION_HPP

#include "avr/interrupt.h"
#include <avr/common.h>

class CriticalSection final {
private:
    uint8_t m_registerSave;
public:
    inline CriticalSection()
        : m_registerSave(SREG)
    {
        cli();
        __asm__ volatile("" ::: );
    }

    inline ~CriticalSection() {
        __asm__ volatile("" ::: "memory"); 
        SREG = m_registerSave;
    }
};

#endif
