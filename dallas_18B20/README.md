# Dallas 18B20 Temperature Sensor Interface

This project provides an interface for the Dallas 18B20 temperature sensor using AVR microcontrollers. The code includes functions to reset the One-Wire bus, write bits, and read bits from the sensor.

## Function: `dallasRead`

### Description

The `dallasRead` function reads the temperature from the Dallas 18B20 sensor.

### Implementation

```c
float dallasRead(void){
    cli();
    asm("WDR");
    uint8_t tempLSB, tempMSB;
    int16_t tempRaw;
    float fTemp;
    if (owi_Reset()){
        owi_WriteByte(0xCC);   // Skip ROM
        owi_WriteByte(0x44);   // Convert T
        _delay_ms(750);
        owi_Reset();
        owi_WriteByte(0xCC);   // Skip ROM
        owi_WriteByte(0xBE);   // Read scratchpad
        tempLSB = owi_ReadByte();
        tempMSB = owi_ReadByte();
        tempRaw = (tempMSB << 8) | tempLSB;
        fTemp = (float)tempRaw / 16.0;
    }
    sei();
    return fTemp;
}