# Low Power 24-Hour Timer on ATtiny13

This project demonstrates how to implement a power-efficient timer on the ATtiny13 microcontroller. The timer can execute a task every 24 hours or at intervals that are multiples of 15 minutes. It achieves this by leveraging the internal Watchdog Timer (WDT) oscillator for time tracking and deep sleep for power efficiency.

## Features

- **Ultra Low Power**: Utilizes the ATtiny13's internal WDT oscillator and deep sleep mode to minimize power consumption.
- **Flexible Timing**: Supports task execution every 24 hours or at intervals of 15 minutes.
- **WDT Calibration**: The WDT oscillator is temperature-dependent and not highly accurate. To compensate, the code recalibrates the WDT tick count every 15 minutes using a timer to ensure precise timing.

## How It Works

1. **Timer0 Setup**: Timer0 is used to generate 10 ms ticks.
2. **WDT Calibration**: At the beginning of every 15-minute period, timer ticks are used to measure the actual WDT timer duration, which is usually between 8 and 9 seconds (for 8-second mode). Then, the code calculates how much additional delay is needed to achieve a perfect 10-second period.
3. **Deep Sleep**: The 24-hour period is composed of `24*4` naps. To achieve a 15-minute period, the microcontroller repeats `90-1` cycles of 10-second breaks, each consisting of three phases:
   - A deep sleep (only the WDT works in interrupt mode),
   - A timer delay (to complete the full 10-second break),
   - Tasks (for example, blinking every 10 seconds, or tasks for a particular loop count, such as every 24 hours).

## Usage

1. Connect an LED to the appropriate pin (PB4 in the example).
2. Set `FINE_DEL_TIME` to 0.
3. Flash the compiled firmware to an ATtiny13 microcontroller.
4. Measure the duration of each 15-minute cycle.
5. Adjust the `OCR0A` value to achieve slightly less than 15 minutes.
6. Adjust `FINE_DEL_TIME` to achieve exactly 15 minutes (usually this value is about 25,000 µs).
7. Set up your task:
   - Use `h` to select the 15-minute period (for example, `h == 0 || h == 4`).
   - Use `i` to track 10-second periods (for example, `i < 6` means 1 minute).

## Files

- `main.c`: The main implementation of the 24-hour timer with WDT calibration.
- `main_1s_blink.c`: A simple example to blink an LED every second.
- `Makefile`: Build and flash instructions for the ATtiny13.



