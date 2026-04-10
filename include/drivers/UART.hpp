#ifndef UART_DRIVER_HPP
#define UART_DRIVER_HPP

#include <avr/io.h>
#include <avr/interrupt.h>

#include "utils/CircularBuffer.hpp"
#include "kernel/Process.hpp"

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

void UART_init();
void UART_write(char c);
char UART_read_blocking();
void UART_print_str(const char* string);

#endif
