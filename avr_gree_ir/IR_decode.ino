#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define BAUD 115200
#include <util/setbaud.h>

#define cbi(sfr, bit) ((sfr) &= ~_BV(bit))
#define sbi(sfr, bit) ((sfr) |= _BV(bit))

volatile uint8_t ir_start = 0;
volatile uint8_t ir_decode = 0;
volatile uint8_t ir_ready = 0;
volatile uint8_t ir_count = 0;

void initUSART(void) {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    UCSR0A |= _BV(U2X0);
    UCSR0B = _BV(TXEN0)  | _BV(RXEN0);
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);  // 8 data bits, 1 stop bit
}
void transmitByte(uint8_t data) {
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = data;
}
void printString(const char myString[]) {
    uint8_t i = 0;
    while (myString[i]) {
        transmitByte(myString[i]);
        ++i;
    }
}

int main(void) {
    initUSART();

    sbi(PORTD, PD2);
    cli();
    EICRA = _BV(ISC00);              // INT0 any change
    EIMSK = _BV(INT0);
    OCR2A = 149;                     // 300 ms
    TCCR2A = _BV(WGM21);             // CTC
    TCCR2B = _BV(CS21) | _BV(CS20);  // prescaler 32
    TIMSK2 = _BV(OCIE2A);
    TCNT2 = 0;
    sei();
    printString("Start. Waiting for IR signal.\n");

    // main loop
    while (1) {
        if (ir_ready) {
            ir_ready=0;
            cli();
            if (ir_decode == 0xFF)
                transmitByte(10);  // new line
            else if (ir_decode < 0b00000111)
                transmitByte(48);  //0
            else
                transmitByte(49);  //1
            sei();
        }
    }
}


ISR(INT0_vect) {
    if (PIND & _BV(PD2)) {      // high - start conversion
        TCNT2 = 0;
        ir_ready = 0;
        ir_decode = 0;
    } else {                    // low - stop, results ready to serial transmition
        ir_ready = 1;
    }
}

ISR(TIMER2_COMPA_vect) {
    ir_decode = (ir_decode << 1);
    if (PIND & _BV(PD2))
        ir_decode |= 1;
}
