#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define OUT_PN  PB4


volatile uint8_t wdt8_tc = 0;
volatile uint16_t ticks_for_quarter = 0;

int main(void) {
    
    cli();
    // enable 8 second WDT
    WDTCR = _BV(WDCE) | _BV(WDE);
    WDTCR = _BV(WDE) | _BV(WDP0) | _BV(WDP3);
    sei();
    //
    asm("wdr");

    DDRB = _BV(OUT_PN); // Set PB4 as output
    //////////////////////////////

    while (1) {
        // toggle every 15 minutes
        asm("wdr");

        // kalibracja czasu
        // Set Timer0 to CTC mode
        TCCR0A |= _BV(WGM01); // CTC mode
        // Set prescaler to 256
        TCCR0B |= _BV(CS02);
        // Adjustest to get 20 Hz
        OCR0A = 120;
        // Enable Timer0 Compare Match A interrupt
        TIMSK0 |= _BV(OCIE0A);

        // WDT as interrupt
        WDTCR |= _BV(WDTIE);
        // START
        wdt8_tc = 0;
        TCNT0 = 0;
        asm("wdr");
        // lock to trigger WDT interrupt
        _delay_ms(10000);
        // now wdt8_tc is around 100 
        asm("wdr");
        uint8_t i;
        for (i=0; i<ticks_for_quarter; ++i){
            MCUCR = _BV(SM1) | _BV(SE);  //power-down | sleep enable
            WDTCR |= _BV(WDTIE);         // WDT as interrupt
            ///////////////
            asm("sleep");
            // wakeup
            asm("wdr");
        }
        PORTB ^=_BV(PB4);
    }
    return (0);
}


ISR(WDT_vect) {
    ticks_for_quarter = 900 * 20 / wdt8_tc - 1;
}

// triggered every 0.05 second
ISR(TIM0_COMPA_vect){
    ++wdt8_tc;
}
