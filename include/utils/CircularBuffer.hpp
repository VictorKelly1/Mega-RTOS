/*
 * @file CircularBuffer.hpp
 * @author Victor Kelly
 * @brief Generic template-based circular buffer (FIFO).
 * Provides a thread-safe-ready data structure for buffering data between 
 * interrupts and user tasks. Ideal for UART, SPI, or I2C communication.
 * @tparam T Type of the elements to be stored.
 * @tparam Size Maximum number of elements (must be power of 2 for optimal performance).
*/

#ifndef CIRCULAR_BUFFER_HPP
#define CIRCULAR_BUFFER_HPP

#include <stdint.h>

/*
 * @class CircularBuffer
 * @brief Circular queue implementation using a fixed-size array.
 * This structure manages two pointers head and tail to provide efficient
*/
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

    /*
     * @brief Adds an item to the front of the buffer (Head).
     * @param item The element to store.
     * @return true if the item was added, false if the buffer is full.
    */
    bool push(T item) {
        if (isFull()) return false;

        m_buffer[m_head] = item;
        m_head = (m_head + 1) % Size;
        m_count++;
        return true;
    }

    /*
     * @brief Removes and retrieves the oldest item from the buffer (Tail).
     * @param item Reference where the popped element will be stored.
     * @return true if an item was retrieved, false if the buffer is empty.
    */
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
