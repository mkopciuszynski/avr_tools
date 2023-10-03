# avr_tools
A bunch of useful AVR implementations.

# Idea
This is a collection of several usuefull avr programs that implement different functionalities like USART serial messages, rotary encoder with a push button, 16x2 LCD and others. 

This is not a ready to use library like those made for Arduino! The idea here is to make something opposite to a library what means that you need to copy manually some parts of the code that is spread among different files. The priority here is to make a final code consistent, what is crucial for microcontrolers. This way you avoid the risk that one library will overwirte some critycal register setup responsible for example for watch dog timmer.

All options are prepared for the Atmega328P microcontroler that is usually installed in Arduino (F_CPU is usually lower than 16 MHz). In some cases you need to adjust the registers accordingly if you use other thype of AVR unit.

# Functionalities

## Serial communication
Implementation of the universal synchronous and asynchronous serial receiver and transmitter (USART) for AVR microcontroller (Atmega328P or similar).


## AVR Rotary encoder
Simple, but not the simplest implementation of the rotary encoder on AVR microcontroller (Atmega328P or similar) with software debouncing.

### Usage
This is not a "ready to use" library. To use this code you need to copy some parts and manullay adjust some parameters like PORT/DDR/PIN name and pin numbers.

### Preliminary assumptions
- microcontroler Atmega328P (or Arduino UNO/Nano with 8 MHz fuses setup)
- Timer0 used for pooling/debouncing
- 8 000 000 Hz clock
- Encoder common pin -> GND
- Encoder A  -> PD5
- Encoder B  -> PD6
- Encoder Switch   -> PD7

