#include "drivers/UART.hpp"
#include "kernel/Kernel.hpp"

// Configuration to avoid static Singletons in AVR
extern "C" {
    __extension__ typedef int __guard __attribute__((mode(__DI__)));
    int __cxa_guard_acquire(__guard *g) { return !*(char *)(g); }
    void __cxa_guard_release(__guard *g) { *(char *)(g) = 1; }
    void __cxa_guard_abort(__guard *) {}
}

// hardware init UART
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

void UART::write(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void UART::print(const char* s) {
    while (*s) {
        write(*s++);
    }
}

void UART::handleInterrupt() {
    char received = UDR0; 
    m_rxBuffer.push(received); 

    if (m_waitingProcess != nullptr) {
        Kernel::getInstance().wakeProcess(m_waitingProcess);
        m_waitingProcess = nullptr; 
    }
}


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
