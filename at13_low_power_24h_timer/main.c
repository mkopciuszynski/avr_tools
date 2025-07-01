#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Pin definitions
#define LED_PN PB4                    // Output pin for LED blinking
#define LED_ON PORTB |= _BV(LED_PN)   // Macro to turn the LED ON
#define LED_OFF PORTB &= ~_BV(LED_PN) // Macro to turn the LED OFF

#define DEV_PN PB3                    // Output pin for an external device
#define DEV_ON PORTB |= _BV(DEV_PN)   // Macro to turn the device ON
#define DEV_OFF PORTB &= ~_BV(DEV_PN) // Macro to turn the device OFF

// Global variables
volatile uint16_t timer_tick, delay_sleep; // Timer tick counter and calculated ticks for delay
volatile uint8_t wdt_lock_flag;            // Flag to synchronize with WDT interrupt

uint8_t h, i; // Loop counters

int main(void)
{
    cli();                                    // Disable global interrupts during setup
    WDTCR = _BV(WDCE) | _BV(WDE);             // Enable Watchdog Timer (WDT) configuration mode
    WDTCR = _BV(WDE) | _BV(WDP0) | _BV(WDP3); // Set WDT timeout to 8 seconds
    sei();                                    // Enable global interrupts
    asm("wdr");                               // Reset the WDT

    // Configure LED and DEV pin as output
    DDRB = _BV(LED_PN) | _BV(DEV_PN);

    // Initial LED blink sequence for startup indication
    for (h = 0; h < 5; ++h)
    {
        LED_ON;
        _delay_ms(500);
        LED_OFF;
        _delay_ms(500);
        asm("wdr"); // Reset the WDT to prevent unwanted reset during delay
    }

    // Main loop
    while (1)
    {
        // Repeat 24*4 times to achieve a 24-hour cycle (each iteration ~15 minutes)
        for (h = 0; h < (24 * 4); ++h)
        {
            asm("wdr");          // Reset the WDT
            WDTCR |= _BV(WDTIE); // Enable WDT interrupt

            // Configure Timer0 in CTC mode for 10 ms period
            // Adjust OCR0A value for timing accuracy
            // OCR0A = 192;  // ~15:27 minutes
            // OCR0A = 186;  // ~14:56 minutes
            OCR0A = 187; // ~15:01 minutes

            // Set Timer0 to CTC (Clear Timer on Compare Match) mode
            TCCR0A |= _BV(WGM01);           // Enable CTC mode
            TCCR0B = _BV(CS00) | _BV(CS01); // Set prescaler to 64
            TIMSK0 |= _BV(OCIE0A);          // Enable Timer0 Compare Match A interrupt
            TCNT0 = 0;                      // Reset Timer0 counter

            LED_ON; // Indicate calibration phase

            // --- WDT Calibration ---
            // Reset the timer tick counter (1 tick per 10 ms)
            timer_tick = 0;
            asm("wdr"); // Reset the WDT
            wdt_lock_flag = 1; // Set lock flag to wait for WDT interrupt
            while (wdt_lock_flag)
                ; // Wait for WDT interrupt to clear the flag

            // WDT takes approximately 8-9 seconds
            // Calculate additional delay to reach exactly 10 seconds (1000 ticks * 10 ms)
            delay_sleep = 1000 - timer_tick;

            LED_OFF; // End of calibration phase

            // --- Wait for the remaining time to complete 10 seconds ---
            timer_tick = 0;
            TCNT0 = 0;
            while (timer_tick < delay_sleep)
                ; // Busy-wait until the remaining time elapses
            TCCR0B = 0; // Stop Timer0

            // --- 15-minute loop (90 cycles of 10 seconds each) ---
            for (i = 0; i < (90 - 1); ++i)
            {
                // Enter deep sleep for approximately 8 seconds (WDT period)
                asm("wdr");
                MCUCR = _BV(SM1) | _BV(SE); // Set MCU to Power-down mode and enable sleep
                WDTCR |= _BV(WDTIE);        // Enable WDT interrupt
                asm("sleep");               // Enter sleep mode

                // Wake up from sleep (WDT interrupt)
                asm("wdr"); // Reset the WDT

                // Wait for the remaining time to complete 10 seconds
                TCCR0B = _BV(CS00) | _BV(CS01); // Set Timer0 prescaler to 64
                timer_tick = 0;
                TCNT0 = 0;
                while (timer_tick < delay_sleep)
                    ; // Busy-wait for the remaining delay

                TCCR0B = 0; // Stop Timer0

                // Blink the LED briefly to indicate activity
                LED_ON;
                _delay_ms(11); // use this dealy for fine adjustment
                LED_OFF;

                ///////////////////////////////////////
                // Task executed once per 24 hours
                // (i < 6) means 6*10 = 60 seconds duration at the start of the first 15-minute period
                if (h == 0 && i < 6)
                {
                    LED_ON;
                    DEV_ON;
                }
                else
                {
                    LED_OFF;
                    DEV_OFF;
                }
                ///////////////////////////////////////
            }
        }
    }

    return 0;
}

// Watchdog Timer interrupt service routine
ISR(WDT_vect)
{
    wdt_lock_flag = 0; // Clear the lock flag to signal WDT period completion
}

// Timer0 Compare Match A interrupt service routine
ISR(TIM0_COMPA_vect)
{
    ++timer_tick; // Increment the timer tick counter (every 10 ms)
}
