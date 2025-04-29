#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define OUT_PN         PB4
#define OUT_ON         PORTB |=  _BV(OUT_PN)
#define OUT_OFF        PORTB &= ~_BV(OUT_PN)
#define SEC_IN_QUARTER  900

volatile uint16_t wdt8_tc, ticks_for_quarter;
volatile uint8_t wdt_lock_flag;
uint8_t i, q;

int main(void) {
    cli();

    // Enable 8-second WDT
    WDTCR = _BV(WDCE) | _BV(WDE);
    WDTCR = _BV(WDE) | _BV(WDP0) | _BV(WDP3);
    sei();

    asm("wdr");
    DDRB = _BV(OUT_PN); // Set PB4 as output

    while (1) {

        // repeat 24*4 times to get 24 hours
        for (q = 0; q < 96; ++q) {

            // Calibrate the WDT timmer
            asm("wdr");
            WDTCR |= _BV(WDTIE); // WDT as interrupt

            // Adjust to get 10 Hz blink frequency
            TCNT0 = 0;
            OCR0A = 240;

            // Set Timer0 to CTC mode
            TCCR0A |= _BV(WGM01);  // CTC mode
            TCCR0B |= _BV(CS02);   // Set prescaler to 256
            TIMSK0 |= _BV(OCIE0A); // Enable Timer0 CMA trigger

            wdt_lock_flag = 1;
            wdt8_tc = 0;
            asm("wdr");
            // Lock to trigger WDT interrupt
            while (wdt_lock_flag)

            // Now wdt8_tc is around 100 for 15 minutes
            // sec_in_quarter * frequency / wdt8_tc - 1
            ticks_for_quarter = (SEC_IN_QUARTER * 20) / wdt8_tc - 1;

            for (i = 0; i < ticks_for_quarter; ++i) {
                MCUCR = _BV(SM1) | _BV(SE);  // Power-down | sleep enable
                WDTCR |= _BV(WDTIE);         // WDT as interrupt
                asm("sleep");
                // Wakeup
                asm("wdr");
                OUT_ON;
                _delay_ms(10);
                OUT_OFF;
            }
        }
    }

    return 0;
}

ISR(WDT_vect) {
    OUT_OFF;
    wdt_lock_flag = 0;
}

// Square-blink frequency 10 Hz / trigger freq 20 Hz
ISR(TIM0_COMPA_vect) {
    ++wdt8_tc;
    PORTB ^= _BV(OUT_PN);
}
