/**
 * @file main_demo.cpp
 * @brief Demo application for megaRTOS v1.0.0
 * * This example demonstrates a system running 6 concurrent tasks (the library supports 
 * up to 7 user tasks + 1 System Idle/Daemon task).
 * * IMPORTANT: Tasks in megaRTOS are event-driven. When a task calls delay() or 
 * UART_read_blocking(), it enters a BLOCKED state, consuming ZERO CPU cycles. 
 * This allows other tasks to run or the system to save power.
 */

#ifndef F_CPU
#define F_CPU 16000000UL // Default for Arduino Uno. Modify if using a different crystal.
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include "kernel/Kernel.hpp"
#include "kernel/Mutex.hpp"
#include "drivers/UART.hpp"

// Global Mutex to protect the UART shared resource
// Prevents message corruption when multiple tasks transmit simultaneously.
Mutex serialMutex;

/**
 * TASK 1: Heartbeat LED (Slow)
 * Demonstrates basic timing using Kernel::delay().
 */
void taskLED_PC0() {
    DDRC |= (1 << PC0); 
    while (true) {
        PINC |= (1 << PC0); 
        Kernel::getInstance().delay(500); // Suspends task for 500ms
    }
}

/**
 * TASK 2: Fast Pulse LED
 * Runs independently of Task 1 with different timing.
 */
void taskLED_PC1() {
    DDRC |= (1 << PC1); 
    while (true) {
        PINC |= (1 << PC1); 
        Kernel::getInstance().delay(250); 
    }
}

/**
 * TASK 3: System Status LED
 * Demonstrates high-level hardware control within the RTOS.
 */
void taskLED_PB5() {
    DDRB |= (1 << PB5); 
    while (true) {
        PINB |= (1 << PB5); 
        Kernel::getInstance().delay(1000); 
    }
}

/**
 * TASK 4: Transmitter A
 * Shares the UART buffer with Task 5. Uses Mutex to ensure atomic printing.
 */
void taskPrintA() {
    while (true) {
        serialMutex.lock();   // Request UART ownership
        UART_print_str("AAAAAA\r\n");
        serialMutex.unlock(); // Release UART ownership
        Kernel::getInstance().delay(100); 
    }
}

/**
 * TASK 5: Transmitter B
 * Competes with Transmitter A for the serial bus.
 */
void taskPrintB() {
    while (true) {
        serialMutex.lock();
        UART_print_str("BBBBBB\r\n");
        serialMutex.unlock();
        Kernel::getInstance().delay(100); 
    }
}

/**
 * TASK 6: Reactive Echo (Receiver)
 * Demonstrates Blocking I/O. This task stays BLOCKED and consumes NO CPU
 * until a character is received via UART, waking up instantly.
 */
void taskEcho() {
    serialMutex.lock();
    UART_print_str("\r\n--- megaRTOS Multi-Tasking Active ---\r\n");
    serialMutex.unlock();

    while (true) {
        // CPU efficient: Task yields control here until a byte arrives
        char c = UART_read_blocking(); 
        
        serialMutex.lock();
        UART_print_str("-> Echo: ");
        UART_write(c);
        UART_print_str("\r\n");
        serialMutex.unlock();
    }
}

int main() {
    // Initialize Hardware drivers
    UART_init();

    // Get Kernel Singleton instance
    Kernel& kernel = Kernel::getInstance();

    // Register up to 7 user tasks
    kernel.createTask(taskLED_PC0, 1);
    kernel.createTask(taskLED_PC1, 1);
    kernel.createTask(taskLED_PB5, 1);
    kernel.createTask(taskPrintA, 1);
    kernel.createTask(taskPrintB, 1);
    kernel.createTask(taskEcho, 1);
    
    // Enable hardware interrupts (Required for Timer and UART)
    sei(); 
    
    // Start the Preemptive Scheduler
    kernel.start();
    
    return 0; // Never reached
}
