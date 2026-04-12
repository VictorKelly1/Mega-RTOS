/*
 * @file UART_driver.hpp
 * @author Victor
 * @brief Interrupt-driven UART driver with task synchronization.
 * This driver utilizes a circular buffer for asynchronous reception and 
 * integrates with the Kernel to block/wake processes for I/O, ensuring zero CPU 
 * overhead while waiting for serial data.
*/

#ifndef UART_DRIVER_HPP
#define UART_DRIVER_HPP

#include <avr/io.h>
#include <avr/interrupt.h>

#include "utils/CircularBuffer.hpp"
#include "kernel/Process.hpp"

/*
 * @class UART
 * @brief Singleton class for hardware UART management.
 * Manages the UART peripheral of the ATmega328P. It handles the 
 * producer-consumer relationship between the RX ISR and user tasks.
*/

/* @brief Internal buffer for incoming serial data. 
 * Size is set to 32 bytes to balance memory usage and data safety.
*/
    
/*@brief Pointer to the process currently waiting for data.
 * Used by the ISR to wake up the specific task when the buffer is no longer empty.
*/
class UART final{
private:
    CircularBuffer<char, 32> m_rxBuffer;

    Process* m_waitingProcess;

    UART()
      :  m_waitingProcess(nullptr)
    {}

public:
    static UART& getInstance(){
        static UART instance;
        return instance;
    }

    void init(uint32_t baudrate);

    char readBlocking();

    void write(char character);
    void print(const char* string);

    void handleInterrupt();
};

/* @brief High-level wrappers to simplify library usage without 
 * directly accessing the UART Singleton instance.
*/
void UART_init();
void UART_write(char c);
char UART_read_blocking();
void UART_print_str(const char* string);

#endif
