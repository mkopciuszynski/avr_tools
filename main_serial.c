/* This is an example code that implements serial communication on Atmega328P or Arduino
 *
 * Author: Marek Kopciuszynski
 */

#define F_CPU 8000000
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
void serByte(uint8_t data);
void serString(const char data[]);


// MAIN function
int main(void){
    cli();
    serInit();
    sei();

    serString("Serial test \r\n");

    // MAIN loop
    while(1){
        if (ser_flag){
            serString("New message: \r\n");
            serString(ser_buff);        // echo received message for test

            memset(ser_buff,0,BUFF_LEN);    // clear serial data buffer
            sb_pointer = 0 ;                // set sb pointer to 0
            ser_flag = 0;                   // set ser_flag to 0
        }
    }
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
void serByte(uint8_t b) {
    while(!(UCSR0A & _BV(UDRE0)));      // Wait for empty transmitter buffer
    UDR0 = b;                           // Put data into buffer and sent it
}
void serString(const char data[]) {
    uint8_t i = 0;
    while (data[i]) {                   // Send byte by byte
        serByte(data[i]);
        ++i;
    }
}


// ISR - serial message received
ISR(USART_RX_vect){
    ser_buff[sb_pointer] = UDR0;        // Get the data from buffer and save in ser_buff
    sb_pointer++;
    ser_flag = 1;
}
