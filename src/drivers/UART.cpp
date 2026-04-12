/*
 * @file UART.cpp
 * @brief Implementation of interrupt-driven UART communication.
 * This file includes the low-level register configuration for ATmega328P
 * and the synchronization logic between hardware interrupts and the Kernel.
*/

#include "drivers/UART.hpp"
#include "kernel/Kernel.hpp"

// avoid static Singletons in AVR
extern "C" {
    __extension__ typedef int __guard __attribute__((mode(__DI__)));
    int __cxa_guard_acquire(__guard *g) { return !*(char *)(g); }
    void __cxa_guard_release(__guard *g) { *(char *)(g) = 1; }
    void __cxa_guard_abort(__guard *) {}
}

/*
 * @brief Configures the USART0 peripheral.
 * Calculation of UBRR follows the standard asynchronous normal mode formula.
 * Enables both Transmitter and Receiver, along with the RX Complete Interrupt.
 * @param baudrate Desired bits per second.
*/
void UART::init(uint32_t baudrate) {
    uint16_t ubrr = (F_CPU / (16 * baudrate)) - 1;
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;

    // TXEN0: enable trasmition 
    // RXEN0: enable reception
    // RXCIE0: enable interruption for complete reception 
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    UCSR0C = (3 << UCSZ00); // Format: 8 bits of data, 1 bit stop 
}

/*
 * @brief Retrieves data from the RX buffer, blocking the task if empty.
 * This is a hybrid approach:
 * 1. It attempts to pop data from the CircularBuffer.
 * 2. If empty, it registers the current process as "waiting" and 
 *    calls the Kernel to suspend execution.
 * 3. Once awakened by the ISR, it loops back to successfully pop the data.
*/
char UART::readBlocking() {
    char data;
    
    while (true) {
        if (m_rxBuffer.pop(data)) {
            return data;
        }

        m_waitingProcess = Kernel::getInstance().getCurrentProcess();
        
        Kernel::getInstance().blockCurrentProcess();
    }
}

/*
 * @brief Reads a character from the buffer.
 * If the buffer is empty, the calling task is BLOCKED by the kernel 
 * until a new character arrives via interrupt.
 * @return char the received character.
*/
void UART::write(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void UART::print(const char* s) {
    while (*s) {
        write(*s++);
    }
}

/*
 * @brief Logic executed upon hardware byte reception.
 * 1. Reads the raw data from the UDR0 register (clears hardware flag).
 * 2. Pushes the byte into the internal CircularBuffer.
 * 3. If a process was blocked waiting for data, it signals the Kernel 
 * to move that process back to the READY state.
*/
void UART::handleInterrupt() {
    char received = UDR0; 
    m_rxBuffer.push(received); 

    if (m_waitingProcess != nullptr) {
        Kernel::getInstance().wakeProcess(m_waitingProcess);
        m_waitingProcess = nullptr; 
    }
}

//Initializations 
void UART_init() { 
    UART::getInstance().init(9600); 
}

char UART_read_blocking() { 
    return UART::getInstance().readBlocking(); 
}

void UART_write(char c) {
    UART::getInstance().write(c);
}

void UART_print_str(const char* s) { 
    UART::getInstance().print(s); 
}

//interruption
ISR(USART_RX_vect) {
    UART::getInstance().handleInterrupt();
}
