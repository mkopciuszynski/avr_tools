/* This is an example code that implements rotary encoder in Atmega328P
 *
 * Author: Marek Kopciuszynski
 */

#define F_CPU 8000000
#define BAUD 9600

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* ======================================
 * Encoder PORT/PIN/DDR
 * ====================================== */
#define ENC_PORT    PORTD
#define ENC_DDR     DDRD
#define ENC_PIN     PIND
#define ENC_A       PD5
#define ENC_B       PD6
#define ENC_S       PD7

volatile int8_t eChange = 0; // encoder change variable
volatile uint8_t bPush = 0;  // button pressed variable
volatile uint8_t bDeb = 0;   // debounce button
volatile uint8_t eDeb = 0;   // debounce encoder

int8_t myVar = 0;            // variable that is changed by the encoder 

// MAIN function
int main(void){
    cli();
    // Encoder timer / timer 0
    // Adjust this value. For smooth response reduce OCR0A
    OCR0A = 30;
    TCCR0A = _BV(WGM01);// CTC mode
    TCCR0B = _BV(CS02); // clk/256 prescaler
    TIMSK0 = _BV(OCIE0A);
    TCNT0  = 0;
    sei();

    // encoder setup pull-up resistors
    ENC_PORT |= _BV(ENC_S)|_BV(ENC_A)|_BV(ENC_B);

    while(1){
        // Encoder support
        if (eChange){
            // Encoder was rotated
            // do some stuff and clean the variable

            myVar += eChange;

            eChange = 0;
        }
        if (bPush){
            // Button was pressed
            // do some stuff and clean the variable
            
            bPush = 0;
        }
        // the rest of code main code
        

    }
}


// Rotary encoder timer
ISR(TIMER0_COMPA_vect){
    // button debouncing
    bDeb = (bDeb<<1);
    if (bit_is_clear(ENC_PIN,ENC_S))    bDeb = bDeb | 1;
    if (bDeb == 0b01111111) bPush=1;
    // rotary encoder debouncing
    eDeb = (eDeb<<1);
    if (bit_is_clear(ENC_PIN,ENC_A))    eDeb = eDeb | 1;
    if (eDeb == 0b00000001){
        if (bit_is_clear(ENC_PIN,ENC_B)){
            --eChange;
        } else {
            ++eChange;
        }
    }
}
