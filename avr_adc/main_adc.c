/* This is an example code that implements serial communication on Atmega328P
 *
 * Author: Marek Kopciuszynski
 */

#define BAUD 9600

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <util/setbaud.h>
#include <stdlib.h>
#include <string.h>

/* ======================================
 * USART variables and functions
 * ====================================== */
#define BUFF_LEN        10          // length of text buffer for received data
char ser_buff[BUFF_LEN];            // serial buffer to store serial messages
volatile uint8_t sb_pointer = 0;    // serial buffer pointer
volatile uint8_t ser_flag = 0;      // serial flag - 1 means new message was received
void serInit(void);
void serSendByte(uint8_t data);
void serSendString(const char data[]);


// ADC variables and functions
float voltage;
float getVoltage(void);

char t[5];
// MAIN function
int main(void){

    cli();
    serInit();
    sei();

    serSendString("Measure the voltage on ADC5. \r\n");

    // MAIN loop
    while(1){
        _delay_ms(5000);

        voltage = getVoltage();
        dtostrf(voltage,3,2,t);
        serSendString(t);
        serSendString("\r\n");

    }

}

/*=================================================================
 *        Measure the voltage on PD5 / internal 1.1V reverence
 *================================================================= */
float getVoltage(void){
    ADMUX  = _BV(REFS0)|_BV(REFS1) ;  // internal reverence 1.1 V with external capacitor
    ADMUX |= _BV(MUX2) | _BV(MUX0);   // select PD5
    ADCSRA |= _BV(ADPS0)|_BV(ADPS2);  // divider by 32
    ADCSRA |= _BV(ADEN);              // enable ADC converter

    uint16_t adcValue = 0;
    uint8_t i;
    _delay_ms(10);                  // wait to stabilise the reference voltage
    for (i=0;i<8;++i){              // read 8 times for better statistic
        ADCSRA |= _BV(ADSC);                        // Start ADC conversion
        loop_until_bit_is_clear(ADCSRA, ADSC);      // Wait
        adcValue += ADC;                            // Read
    }
    ADCSRA &= ~_BV(ADEN);           // stop ADC
    ADMUX = 0x00;
    // divide the result by 8
    adcValue = adcValue / 8;
    // voltage divider made of 220k and 10k resistors
    float fVoltage = adcValue * 0.024; // convert to real unit
    return fVoltage;
}

/* ======================================
 * USART variables and functions
 * ====================================== */
void serInit(void){
    // Init and setup USART
    UCSR0B = _BV(RXEN0)  | _BV(TXEN0);      // Enable receiver and transmitter
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);     // 8 data bits, 1 stop bit
    UBRR0H = UBRRH_VALUE;                   // set the baud rate
    UBRR0L = UBRRL_VALUE;
    UCSR0B |= _BV(RXCIE0);                  // set RX complete interrupt
}
void serSendByte(uint8_t b) {
    while(!(UCSR0A & _BV(UDRE0)));      // Wait for empty transmitter buffer
    UDR0 = b;                           // Put data into buffer and sent it
}
void serSendString(const char data[]) {
    uint8_t i = 0;
    while (data[i]) {                   // Send byte by byte
        serSendByte(data[i]);
        ++i;
    }
}


// ISR - serial message received
ISR(USART_RX_vect){
    ser_buff[sb_pointer] = UDR0;        // Get the data from buffer and save in ser_buff
    sb_pointer++;
    ser_flag = 1;
}
