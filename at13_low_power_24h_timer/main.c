#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Pin definitions
#define OUT_PN         PB4                // Output pin for blinking
#define OUT_ON         PORTB |=  _BV(OUT_PN)  // Macro to turn the output ON
#define OUT_OFF        PORTB &= ~_BV(OUT_PN)  // Macro to turn the output OFF
#define SEC_IN_SLEEP   900                // Sleep duration in seconds (15 minutes)

// Global variables
volatile uint16_t wdt8_tc, ticks_for_quarter; // WDT tick counter and calculated ticks for a quarter
volatile uint8_t wdt_lock_flag;              // Flag to lock WDT operation
uint8_t i, q;                                // Loop counters

int main(void) {

    cli(); // Disable global interrupts during setup

    // Configure and enable the 8-second Watchdog Timer (WDT)
    WDTCR = _BV(WDCE) | _BV(WDE);           // Enable WDT configuration mode
    WDTCR = _BV(WDE) | _BV(WDP0) | _BV(WDP3); // Set WDT timeout to 8 seconds
    sei(); // Enable global interrupts
    asm("wdr"); // Reset the WDT

    // Configure OUT_PN as an output pin
    DDRB = _BV(OUT_PN);
    
    // Turn ON the output for 3 seconds at startup for initialization indication
    OUT_ON;
    _delay_ms(3000);
    OUT_OFF;

    // Main loop
    while (1) {

        // Repeat 24*4 times to achieve a 24-hour cycle (15 minutes per iteration)
        for (q = 0; q < 24*4; ++q) {

            // Calibrate the WDT timer
            asm("wdr"); // Reset the WDT
            WDTCR |= _BV(WDTIE); // Enable WDT interrupt

            // Configure Timer0 for 20 Hz trigger frequency
            // Adjust OCR0A value for accuracy
            // if the clock is too slow this value should be increased
            OCR0A = 251; //
            
            // Set Timer0 to CTC (Clear Timer on Compare Match) mode
            TCCR0A |= _BV(WGM01);  // Enable CTC mode
            TCCR0B |= _BV(CS02);   // Set prescaler to 256
            TIMSK0 |= _BV(OCIE0A); // Enable Timer0 Compare Match A interrupt
            TCNT0 = 0;             // Reset Timer0 counter
            
            // Reset the WDT tick counter
            wdt8_tc = 0;
            asm("wdr"); // Reset the WDT
            wdt_lock_flag = 1; // Lock to wait for WDT interrupt
            while (wdt_lock_flag); // Wait until WDT interrupt unlocks

            TCCR0B = 0; // Stop Timer0

            // Calculate the number of ticks for a 15-minute quarter
            // Formula: (sleep duration in seconds * frequency) / WDT ticks - 1
            ticks_for_quarter = ((SEC_IN_SLEEP * 20) / wdt8_tc) - 1;

            // Perform the sleep-wake cycle for the calculated number of ticks
            for (i = 0; i < ticks_for_quarter; ++i) {
                MCUCR = _BV(SM1) | _BV(SE);  // Set MCU to Power-down mode and enable sleep
                WDTCR |= _BV(WDTIE);         // Enable WDT interrupt
                asm("sleep");                // Enter sleep mode
                // Wakeup from sleep
                asm("wdr");                  // Reset the WDT
                OUT_ON;                      // Blink the output
                _delay_ms(10);               // Fine-tune the blink duration
                OUT_OFF;                     // Turn off the output
            }
        }
    }

    return 0;
}

// Watchdog Timer interrupt service routine
ISR(WDT_vect) {
    OUT_OFF;            // Ensure the output is turned off
    wdt_lock_flag = 0;  // Unlock the WDT lock flag
}

// Timer0 Compare Match A interrupt service routine
// Used to generate a square wave with a frequency of 10 Hz (toggle frequency)
ISR(TIM0_COMPA_vect) {
    ++wdt8_tc;          // Increment the WDT tick counter
    PORTB ^= _BV(OUT_PN); // Toggle the output pin
}
