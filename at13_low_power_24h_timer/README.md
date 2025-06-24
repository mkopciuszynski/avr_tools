# Low Power 24-Hour Timer on ATtiny13

This project demonstrates how to implement a power-efficient timer on the ATtiny13 microcontroller. The timer can execute a task every 24 hours or at intervals that are multiples of 15 minutes. It achieves this by leveraging the internal Watchdog Timer (WDT) oscillator for time tracking and deep sleep for power efficiency.

## Features

- **Ultra Low Power**: Utilizes the ATtiny13's internal WDT oscillator and deep sleep mode to minimize power consumption.
- **Flexible Timing**: Supports task execution every 24 hours or at intervals of 15 minutes.
- **WDT Calibration**: The WDT oscillator is temperature-dependent and not highly accurate. To compensate, the code recalibrates the WDT tick count every 15 minutes using a timer to ensure precise timing.

## How It Works

1. **Timer0 setup**: Timer0 is used to trigger 10 ms ticks.
2. **WDT Calibration**: At the beggining of every 15 minutes period, timmer ticks are used to measure the real WDT timer time that is usually between 8-9 seconds (for 8 second mode). Then we calculate for how long do we need to continue the break to get perfect 10 second period.
3. **Deep Sleep**: 24 hours period is composed of `24*4` naps. To get 15 minutes period the microcontroller repeats `90-1` times the 10 seconds breaks that are composed of three phases:
- a deep sleep (only WDT wordks in interrupt mode),
- a timmer break (to have full 10 s break),
- tasks (for example blinking every 10 s, or tasks for particular loop count - i.e. every 24 h).

## Usage

1. Connect an LED to the appropriate pin (PB4 in the example).
2. Set `FINE_DEL_TIME` to 0.
3. Flash the compiled firmware to an ATtiny13 microcontroller.
3. Measure the time of each 15 minutes cycle.
4. Adjust `OCRA0A` value to have slightly less than 15 minutes.
5. Adjust the `FINE_DEL_TIME` to have perfect 15 minutes (usually this value is about 25000 us)
6. Setup the task:
- use `h` to select the 15 minutes period (for example `h==0 && h==4`)
- use `i` to track 10 second periods (if i < 6 means 1 minute).

## Files

- `main.c`: The main implementation of the 24-hour timer with WDT calibration.
- `main_1s_blink.c`: A simple example to blink an LED every second.
- `Makefile`: Build and flash instructions for the ATtiny13.



