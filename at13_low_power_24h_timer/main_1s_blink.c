#include <avr/io.h>
#include <util/delay.h>

#define LED_PN         PB4

int main(void) {
    DDRB = _BV(OUT_PN); // Set PB4 as output
    while (1) {
        PORTB ^= _BV(OUT_PN); // Toggle PB4
        _delay_ms(1000);   // Delay for 1 second
    }
    return 0;
}
