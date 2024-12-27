#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
////////////////////////////
#define sbi(port, pin) (port) |= _BV(pin)
#define cbi(port, pin) (port) &= ~_BV(pin)
#define tbi(port, pin) (port) ^= _BV(pin)
///////////////////////////////////////////////////////////
// ID LED connected to the PB3 pint
#define IR_PORT PORTB
#define IR_DDR  DDRB
#define IR_PN  PB3
///////////////////////////////////////////////////////////
#define IR_900 684 // =9ms/(0.5/34kHz)
#define IR_450 342
#define IR_CON 1850
#define IR_CON2 3100
#define IR_H 48
#define IR_L0 43
#define IR_L1 127
#define IR_ON  sbi(TCCR2A,COM2A0); ir_count = 0
#define IR_OFF cbi(TCCR2A,COM2A0); ir_count = 0
volatile uint_fast16_t ir_count = 0;
///////////////////////////////////////////////////////////
// Example ir_codes used for heating 
uint8_t ir_data_20C[27] = {
    0b00111010,0b00100000,0b00000110,0b00001010,0b01000000,
    0b10001000,0b00000011,0b00000000,0b00001110,
    0b00111010,0b00100000,0b00000110,0b00001110,0b01000000,
    0b00000000,0b00000000,0b00001000,0b00001101,
    0b00000000,0b00000000,0b00000000,0b00000101,0b01000000,
    0b00000000,0b00000000,0b00000000,0b00000101};
uint8_t ir_data_18C[27] = {
    0b00111010,0b01000000,0b00000110,0b00001010,0b01000000,
    0b10001000,0b00000011,0b00000000,0b00001010,
    0b00111010,0b01000000,0b00000110,0b00001110,0b01000000,
    0b00000000,0b00000000,0b00001000,0b00001001,
    0b00000000,0b00000000,0b00000000,0b00000101,0b01000000,
    0b00000000,0b00000000,0b00000000,0b00000101};
uint8_t ir_data_16C[27] = {
    0b00111010,0b00000000,0b00000110,0b00001010,0b01000000,
    0b10001000,0b00000011,0b00000000,0b00001100,
    0b00111010,0b00000000,0b00000110,0b00001110,0b01000000,
    0b00000000,0b00000000,0b00001000,0b00001110,
    0b00000000,0b00000000,0b00000000,0b00000101,0b01000000,
    0b00000000,0b00000000,0b00000000,0b00000101};
uint8_t ir_data_stop[27] = {
    0b00101010,0b00000000,0b00000100,0b00001010,0b01000000,
    0b10001000,0b00000011,0b00000000,0b00001101,
    0b00101010,0b00000000,0b00000100,0b00001110,0b01000000,
    0b00000000,0b00000000,0b00001000,0b00001111,
    0b00000000,0b00000000,0b00000000,0b00000101,0b01000000,
    0b00000000,0b00000000,0b00000000,0b00000101};
///////////////////////////////////////////////////////////
void ir_send(uint8_t *ir_data);
///////////////////////////////////////////////////////////
int main(void){

    while(1){
        // test the library
        ir_send(ir_data_16C);
        _delay_ms(5000);
        ir_send(ir_data_18C);
        _delay_ms(5000);
        ir_send(ir_data_20C);
        _delay_ms(5000);

    }

}


void ir_send(uint8_t *ir_data){
    int_fast8_t i,j,p;
    uint_fast8_t buff;
    sbi(IR_DDR,IR_PN);
    cli();
    // Stop suspend timers 0 and 1
    cbi(TIMSK0,OCIE0A);
    cbi(TIMSK1,OCIE1A);
    // Set timer 2 to produce 38 kHz square signal
    OCR2A = 12;
    TCCR2A = _BV(WGM21);
    TCCR2B = _BV(CS21);
    TIMSK2 = _BV(OCIE2A);
    TCNT2 = 0;
    sei();
    // main loop of the library
    p = 0;
    while (p < (3*9)) {
        // sent full package with 3x9 bytes
        j=0;
        // Start signal - high for 9 ms
        IR_ON;   while (ir_count<IR_900);
        // Start signal - low for 4.5 ms
        IR_OFF;  while (ir_count<IR_450);
        while (j < 9) {
            buff = ir_data[j+p];
            for (i=7; i>=0; --i){
                IR_ON;
                while (ir_count<IR_H);
                IR_OFF;
                if (buff & _BV(i)) while (ir_count<IR_L1);
                else               while (ir_count<IR_L0);
                if (j==4){
                    if (i==5) {
                        IR_ON;   while (ir_count<IR_H);
                        IR_OFF;  while (ir_count<IR_CON);
                        break;
                    }
                }
            }
            ++j;
        }
        IR_ON;   while (ir_count<IR_H);
        IR_OFF;  while (ir_count<IR_CON2);
        p = p+9;
    }
    TCCR2B = 0x00;
    sbi(TIMSK0,OCIE0A);
    sbi(TIMSK1,OCIE1A);
}

// IR timer
ISR(TIMER2_COMPA_vect){
    ++ir_count;
}

