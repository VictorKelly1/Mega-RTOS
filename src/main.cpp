#include <avr/io.h>
#include "kernel/Kernel.hpp"

void task1() {
    DDRC |= (1 << PC0);

    while (true) {
        PORTC ^= (1 << PC0);
        Kernel::getInstance().delay(500); 
    }
}

void idleTask(){
    while(true){

    }
}

void task2() {
    DDRC |= (1 << PC1);

    while (true) {
        PORTC ^= (1 << PC1);
        Kernel::getInstance().delay(500);
    }
}

int main() {
    Kernel& kernel = Kernel::getInstance();

    kernel.createTask(task1, 1);
    kernel.createTask(task2, 2);
    kernel.createTask(idleTask, 1);

    kernel.start();

    while (1) {
        asm volatile("nop");
    }

    return 0;
}
