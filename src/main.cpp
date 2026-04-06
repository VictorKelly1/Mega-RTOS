#include <avr/io.h>
#include <util/delay.h> 
#include "kernel/Kernel.hpp"

void busyDelay(uint16_t ms) {
    for (uint16_t i = 0; i < ms; i++) {
        _delay_ms(1); 
    }
}

void task1() {
    DDRC |= (1 << PC0);

    while (true) {
        PORTC ^= (1 << PC0);
        busyDelay(500);
    }
}

void task2() {
    DDRC |= (1 << PC1);

    while (true) {
        PORTC ^= (1 << PC1);
        busyDelay(250);
    }
}

int main() {
    Kernel& kernel = Kernel::getInstance();

    kernel.createTask(task1, 1);
    kernel.createTask(task2, 2);

    kernel.start();

    while (1) {
        asm volatile("nop");
    }

    return 0;
}

