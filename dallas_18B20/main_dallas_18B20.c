/* This code implements basic Dallas 18B20 communication 
 * with simple temperature read
 *
 * Author: Marek Kopciuszynski
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Dallas variables and macros
float fTemp = 25.5;
#define OWI_PN      PB2
#define OWI_PORT    PORTB
#define OWI_DDR     DDRB
#define OWI_PIN     PINB
#define OWI_PULL_BUS_LOW  OWI_DDR |= _BV(OWI_PN); OWI_PORT &=~_BV(OWI_PN);
#define OWI_RELEASE_BUS   OWI_DDR &=~_BV(OWI_PN); OWI_PORT &=~_BV(OWI_PN);
uint8_t owi_Reset(void);
void owi_WriteBit(uint8_t b);
void owi_WriteByte(uint8_t b);
uint8_t owi_ReadBit(void);
uint8_t owi_ReadByte(void);
float dallasRead(void);

// LED in Arduino for basic notivication to check if it works
#define LED_DDR         DDRB
#define LED_PORT        PORTB  
#define LED_PN          PB5
#define LED_ON          LED_PORT |=  _BV(LED_PN)
#define LED_OFF         LED_PORT &= ~_BV(LED_PN)

// MAIN function
int main(void){
    LED_DDR |= _BV(LED_PN); 

    // MAIN loop
    while(1){
        fTemp = dallasRead();

        if (fTemp>20 && fTemp<30) LED_ON;
        else LED_OFF;

        _delay_ms(1000);
    }
}

/*======================================
* OWI Dallas
* ====================================== */
uint8_t owi_Reset(void){
    uint8_t presenceConf = 0;
    OWI_PULL_BUS_LOW;   _delay_us(600);
    OWI_RELEASE_BUS;    _delay_us(70);
    if (bit_is_clear(OWI_PIN,OWI_PN)) presenceConf = 1;
    _delay_us(450);
    return presenceConf;
}

void owi_WriteBit(uint8_t bit){
    OWI_PULL_BUS_LOW;
    _delay_us(2);
    if (bit) OWI_RELEASE_BUS;
    _delay_us(70);
    OWI_RELEASE_BUS;
}

uint8_t owi_ReadBit(void){
    uint8_t bit = 0;
    OWI_PULL_BUS_LOW;   _delay_us(2);
    OWI_RELEASE_BUS;    _delay_us(10);
    if (bit_is_set(OWI_PIN, OWI_PN)) bit = 1;
    _delay_us(60);
    return bit;
}

void owi_WriteByte(uint8_t byte){
    for (uint8_t i=0;i<8;++i){
        owi_WriteBit(byte & (1<<i));
    }
}

uint8_t owi_ReadByte(void){
    uint8_t byte = 0;
    for (uint8_t i=0; i<8; ++i)
        byte |= (owi_ReadBit() << i);
    return byte;
}

float dallasRead(void){
    cli();
    asm("WDR");
    uint8_t tempLSB,tempMSB;
    int16_t tempRaw;
    float fTemp;
    if (owi_Reset()){
        owi_WriteByte(0xCC);   //Skip ROM
        owi_WriteByte(0x44);   //convert T
        _delay_ms(750);
        owi_Reset();
        owi_WriteByte(0xCC);   //Skip ROM
        owi_WriteByte(0xBE);   //read scratchpad
        tempLSB = owi_ReadByte();
        tempMSB = owi_ReadByte();
        tempRaw = (tempMSB << 8) | tempLSB;
        fTemp = tempRaw / 16.0;
    } else {
        fTemp = 15.5;
    }
    sei();
    return fTemp;
}