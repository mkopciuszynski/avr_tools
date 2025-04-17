#include <avr/io.h>
#include <avr/interrupt.h>

#define OUT_PN         PB4

volatile uint16_t timer_ticks = 0;


int main(void) {
    cli();
    // Set Timer0 to CTC mode
    TCCR0A |= (1 << WGM01); // CTC mode

    // Set prescaler to 256
    TCCR0B |= (1 << CS02);

    // Calculate and set the OCR0A value for 20 Hz
    // Formula: OCR0A = (F_CPU / (2 * Prescaler * Target Frequency)) - 1
    // Adjust to get exactly 20 Hz (use oscilloscope)
    OCR0A = 120;

    // Enable Timer0 Compare Match A interrupt
    TIMSK0 |= (1 << OCIE0A);
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
