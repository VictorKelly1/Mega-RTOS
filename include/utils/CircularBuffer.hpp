#ifndef CIRCULAR_BUFFER_HPP
#define CIRCULAR_BUFFER_HPP

#include <stdint.h>

template <typename T, uint8_t Size>
class CircularBuffer {
private:
    T m_buffer[Size];
    uint8_t m_head;         //top of the queue 
    uint8_t m_tail;         //down of the queue 
    uint8_t m_count;        //number of elements in queue 

public: 
    CircularBuffer() 
      : m_head(0)
      , m_tail(0)
      , m_count(0) 
    {}

    bool push(T item) {
        if (isFull()) return false;

        m_buffer[m_head] = item;
        m_head = (m_head + 1) % Size;
        m_count++;
        return true;
    }

    bool pop(T& item) {
        if (isEmpty()) return false;

        item = m_buffer[m_tail];
        m_tail = (m_tail + 1) % Size;
        --m_count;
        return true;
    } 

    bool isEmpty() const { return m_count == 0; }
    bool isFull() const { return m_count == Size; }

    uint8_t count() const { return m_count; }
};

#endif
