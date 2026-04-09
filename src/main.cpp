#include <avr/io.h>
#include "kernel/Kernel.hpp"
#include "kernel/Mutex.hpp"

Mutex serialMutex;

void UART_init() {
    UBRR0H = 0;
    UBRR0L = 103; // 9600 
    UCSR0B = (1 << TXEN0);
    UCSR0C = (3 << UCSZ00);
}

void task1() {
    DDRC |= (1 << PC0);

    while (true) {
        PORTC ^= (1 << PC0);
        Kernel::getInstance().delay(500); 
    }
}

void task2() {
    DDRC |= (1 << PC1);

    while (true) {
        PORTC ^= (1 << PC1);
        Kernel::getInstance().delay(500);
    }
}

void UART_print_str(const char* s) {
    while (*s) {
        while (!(UCSR0A & (1 << UDRE0)));
        UDR0 = *s++;
    }
}

void taskA() {
    while (true) {
        serialMutex.lock();
        UART_print_str("AAAAAA\r\n");
        serialMutex.unlock();
       
        Kernel::getInstance().delay(15); 
    }
}

void taskB() {
    while (true) {
        serialMutex.lock();
        UART_print_str("BBBBBB\r\n");
        serialMutex.unlock();
        
        Kernel::getInstance().delay(15); 
    }
}

void taskC() {
    while (true) {
        serialMutex.lock();
        UART_print_str("CCCCCC\r\n");
        serialMutex.unlock();
        
        Kernel::getInstance().delay(15); 
    }
}

int main() {
    UART_init();
    
    Kernel::getInstance().createTask(task1, 1);
    Kernel::getInstance().createTask(task2, 1);
    Kernel::getInstance().createTask(taskA, 1);
    Kernel::getInstance().createTask(taskB, 1);
    Kernel::getInstance().createTask(taskC, 1);
    
    Kernel::getInstance().start();
    return 0;
}


/*#include <avr/io.h>
#include "kernel/Kernel.hpp"

void task1() {
    DDRC |= (1 << PC0);

    while (true) {
        PORTC ^= (1 << PC0);
        Kernel::getInstance().delay(500); 
    }
}

void task2() {
    DDRC |= (1 << PC1);

    while (true) {
        PORTC ^= (1 << PC1);
        Kernel::getInstance().delay(500);
    }
}

void task3() {
    DDRB |= (1 << PB5);

    while (true) {
        PORTB ^= (1 << PB5);
        Kernel::getInstance().delay(500);
    }
}

int main() {
    Kernel& kernel = Kernel::getInstance();

    kernel.createTask(task1, 1);
    kernel.createTask(task2, 2);
    kernel.createTask(task3, 0);

    kernel.start();


    return 0;
}*/ 
