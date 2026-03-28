#include <avr/io.h>

int main() {
    DDRB |= (1 << PB5);

    while (1) {
        PORTB ^= (1 << PB5);
        for (volatile long i = 0; i < 100000; i++);
    }
}
