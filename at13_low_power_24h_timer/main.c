#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Pin definitions
#define LED_PN PB4                    // Output pin (for blinking)
#define LED_ON PORTB |= _BV(LED_PN)   // Macro to turn the output ON
#define LED_OFF PORTB &= ~_BV(LED_PN) // Macro to turn the output OFF

#define DEV_PN PB3
#define DEV_ON PORTB |= _BV(DEV_PN)
#define DEV_OFF PORTB &= ~_BV(DEV_PN)


#define FINE_DEL_TIME 26800 // Fine time tunning

// Global variables
volatile uint16_t timer_tick, delay_sleep;    // WDT tick counter and calculated ticks for SEC_IN_SLEEP
volatile uint8_t wdt_lock_flag;               // Flag to lock WDT operation

uint8_t h, i;                                 // Loop counters

int main(void)
{

    cli();                                    // Disable global interrupts
    WDTCR = _BV(WDCE) | _BV(WDE);             // Enable WDT configuration mode
    WDTCR = _BV(WDE) | _BV(WDP0) | _BV(WDP3); // Set WDT timeout to 8 seconds
    sei();                                    // Enable global interrupts
    asm("wdr");                               // Reset the WDT

    // Configure OUT_PN as an output pin
    DDRB = _BV(LED_PN);

    // Inint blink
    for (h = 0; h < 5; ++h)
    {
        LED_ON;
        _delay_ms(200);
        LED_OFF;
        _delay_ms(200);
        asm("wdr");
    }

    // Main loop
    while (1)
    {

        // Repeat 24*4 times to achieve a 24-hour cycle
        for (h = 0; h < (24 * 2); ++h)
        {

            asm("wdr");          // Reset the WDT
            WDTCR |= _BV(WDTIE); // Enable WDT interrupt

            // Configure Timer0, CTC, 10 ms period
            OCR0A = 195; // Adjust OCR0A value for accuracy

            // Set Timer0 to CTC (Clear Timer on Compare Match) mode
            TCCR0A |= _BV(WGM01);           // Enable CTC mode
            TCCR0B = _BV(CS00) | _BV(CS01); // preskaler 64
            TIMSK0 |= _BV(OCIE0A);          // Enable Timer0 Compare Match A interrupt
            TCNT0 = 0;                      // Reset Timer0 counter

            LED_ON;

            // First, measure accurate the WDT time
            // Reset the WDT tick counter (1 tick per 10 ms)
            timer_tick = 0;
            asm("wdr"); // Reset the WDT
            wdt_lock_flag = 1;
            // Lock to wait for WDT interrupt
            while (wdt_lock_flag)
                ;

            // WDT takes approximately 8-9 seconds
            // Calculate to get 10 seconds (1000 ticks * 10 ms)
            delay_sleep = 1000 - timer_tick;

            LED_OFF;

            // Wait for full 10 sedonds
            timer_tick = 0;
            TCNT0 = 0;
            while (timer_tick < delay_sleep)
                ;
            TCCR0B = 0; // Stop Timer0

            // WDT calibration done
            // 15 minutes loop
            for (i = 0; i < (90 - 1); ++i)
            {
                // Enter the deep sleep for aprox. 8 sedonds
                asm("wdr");
                MCUCR = _BV(SM1) | _BV(SE); // Set MCU to Power-down mode and enable sleep
                WDTCR |= _BV(WDTIE);        // Enable WDT interrupt
                asm("sleep");               // Enter sleep mode

                // Wakeup from sleep
                asm("wdr"); // Reset the WDT

                // Wait approx. 2 more seconds
                TCCR0B = _BV(CS00) | _BV(CS01); // preskaler 64
                timer_tick = 0;
                TCNT0 = 0;
                while (timer_tick < delay_sleep)
                    ;

                TCCR0B = 0; // Stop Timer0

                // Use adjustable delay for better time tunning
                _delay_us(FINE_DEL_TIME);

                LED_ON; // Blink the output
                _delay_ms(20);
                LED_OFF;

                ///////////////////////////////////////
                // one minute time once per 24 h
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
    wdt_lock_flag = 0; // Unlock the WDT lock flag
}

// Timer0 Compare Match A interrupt service routine
ISR(TIM0_COMPA_vect)
{
    ++timer_tick; // Increment the WDT tick counter
    // PORTB ^= _BV(OUT_PN); // Toggle the output pin
}
