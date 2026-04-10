
#include <avr/io.h>
#include <avr/interrupt.h>
#include "kernel/Kernel.hpp"
#include "kernel/Mutex.hpp"
#include "drivers/UART.hpp"

// Instancia global del Mutex para proteger el puerto Serial
Mutex serialMutex;


void taskLED_PC0() {
    DDRC |= (1 << PC0); // Pin A0
    while (true) {
        PINC |= (1 << PC0); 
        Kernel::getInstance().delay(500); 
    }
}

void taskLED_PC1() {
    DDRC |= (1 << PC1); // Pin A1
    while (true) {
        PINC |= (1 << PC1); 
        Kernel::getInstance().delay(250); 
    }
}

void taskLED_PB5() {
    DDRB |= (1 << PB5); // Pin 13
    while (true) {
        PINB |= (1 << PB5); 
        Kernel::getInstance().delay(1000); 
    }
}


void taskPrintA() {
    while (true) {
        serialMutex.lock();
        UART_print_str("AAAAAA\r\n");
        serialMutex.unlock();
        Kernel::getInstance().delay(100); 
    }
}

void taskPrintB() {
    while (true) {
        serialMutex.lock();
        UART_print_str("BBBBBB\r\n");
        serialMutex.unlock();
        Kernel::getInstance().delay(100); 
    }
}


void taskEcho() {
    serialMutex.lock();
    UART_print_str("\r\n[SISTEMA MULTI-TAREA ACTIVO]\r\n");
    serialMutex.unlock();

    while (true) {
        // Esta tarea se duerme y libera el CPU hasta que presiones una tecla
        char c = UART_read_blocking(); 
        
        serialMutex.lock();
        UART_print_str("-> Eco: ");
        UART_write(c);
        UART_print_str("\r\n");
        serialMutex.unlock();
    }
}

int main() {
    UART_init();
    Kernel& kernel = Kernel::getInstance();

    kernel.createTask(taskLED_PC0, 1);
    kernel.createTask(taskLED_PC1, 1);
    kernel.createTask(taskLED_PB5, 1);
    
    kernel.createTask(taskPrintA, 1);
    kernel.createTask(taskPrintB, 1);
    
    kernel.createTask(taskEcho, 1);
    
    sei(); 
    
    kernel.start();
    
    return 0;
}

