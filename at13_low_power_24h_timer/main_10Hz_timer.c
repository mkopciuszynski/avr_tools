#include <avr/io.h>
#include <avr/interrupt.h>

#define OUT_PN         PB4

volatile uint16_t timer_ticks = 0;


int main(void) {
    cli();

    // Set Timer0 to CTC mode
    TCCR0A |= _BV(WGM01); // CTC mode
    // Set prescaler to 256
    TCCR0B |= _BV(CS02);
    // Adjustest to get 10 Hz
    OCR0A = 240;
    // Enable Timer0 Compare Match A interrupt
    TIMSK0 |= _BV(OCIE0A);

    // START
    TCNT0 = 0;
    sei();

    DDRB = _BV(OUT_PN); // Set PB4 as output
    
    while (1) {
    }
    return 0;
}


ISR(TIM0_COMPA_vect){
    ++timer_ticks;
    PORTB ^=  _BV(OUT_PN);
}
