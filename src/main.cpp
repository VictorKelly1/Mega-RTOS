
#include <avr/io.h>

#include "kernel/Kernel.hpp"

void task1(){
    DDRC |= (1 << PC0);
    PORTC |= (1 << PC0);

}

void task2(){
    DDRC |= (1 << PC1);
    PORTC |= (1 << PC1);

}

int main() {

    Kernel& kernel = Kernel::getInstance();

    kernel.createTask(task1, 1);
    kernel.createTask(task2, 2);

    kernel.start();

    while (1) {
        //
    }
}
