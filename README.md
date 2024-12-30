# AVR Tools
A bunch of useful AVR implementations.

Rotary encoder with a push button, 16x2 LCD, and others.

This is not a ready-to-use library like those made for Arduino! The idea here is to make something opposite to a library, which means that you need to manually copy some parts of the code that are spread among different files. The priority here is to make the final code consistent, which is crucial for microcontrollers. This way, you avoid the risk that one library will overwrite some critical register setup responsible, for example, for the watchdog timer.

All options are prepared for the Atmega328P microcontroller that is usually installed in Arduino (F_CPU is usually lower than 16 MHz - 8 MHz typically). In some cases, you need to adjust the registers accordingly if you use another type of AVR unit.

## Resources
- [ADC converter in AVR controller](./adc/README.md)
- [Dallas 18B20 Temperature Sensor Interface](./dallas_18B20/README.md)
- [IR Gree air conditioner communication](./gree_ir/README.md)
- [Rotary encoder with push button (debounced)](./rotary_encoder/README.md)

### Usage
This is not a "ready to use" library. To use this code, you need to copy some parts and manually adjust some parameters like PORT/DDR/PIN names and pin numbers.

### Preliminary assumptions
- Microcontroller: Atmega328P (or Arduino UNO/Nano with 8 MHz fuse setup)
- 8,000,000 Hz clock
