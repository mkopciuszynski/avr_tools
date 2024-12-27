# AVR Gree IR remote control
Simple AVR library for Gree Air conditioner remote IR control.

## Idea
This is a bare C code for avr microcontrollers, especially Atmega328P/Arduino to control Gree air conditioner using dedicated IR protocol. The IR codes could be recorder using Arduino IR decoder that is part of this repository.

## Assumptions
- Atmega 328P set to use 8 MHz internal oscillator.
- Infrared LED connected to PB3 pin via about 100 Ohm (depending on power voltage) resistor.
- Timer 2 is used to generate 38 kHz IR carrier signal.
- IR controlling codes are first recorder using separate tool and saved as an array in memory.

# Arduino IR decoder
The check sum that is used in Gree protocol is tricky to calculate so I prefer to first record the signal I need, save it in the memory and use when needed. For this reason I use simple decoder build using Arduino.

## Assumptions
-	IR receiver output connected to PD2 (D2 Arduino pin).
-	Arduino Nano or Uno (with atmega328p) connected to the computer with Arduino serial monitor running.

## How to use
Just select the function you want to decode on the original IR remote control, and press it several times to see the code. Next the code needs to be transformed into array. 
Please note that each frame of data is separated into two parts with the gap between. Moreover in first segment only 35 bits are sent. That means we have 4 bytes and only 3 bits from the last 5-th byte. Typically (however it depends on the version of gree air conditioner) there are three segments/frames, but the library could be easily modified to fix particular needs.

