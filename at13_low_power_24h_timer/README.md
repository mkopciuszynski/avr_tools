# Low Power 24-Hour Timer on ATtiny13

This project demonstrates how to implement a super power-efficient timer on the ATtiny13 microcontroller. The timer can execute a task every 24 hours or at intervals that are multiples of 15 minutes. It achieves this by leveraging the internal Watchdog Timer (WDT) oscillator for time tracking and deep sleep for power efficiency.

## Features

- **Ultra Low Power**: Utilizes the ATtiny13's internal WDT oscillator and deep sleep mode to minimize power consumption.
- **Flexible Timing**: Supports task execution every 24 hours or at intervals of 15 minutes.
- **WDT Calibration**: The WDT oscillator is temperature-dependent and not highly accurate. To compensate, the code recalibrates the WDT tick count every 15 minutes using a timer to ensure precise timing.

## How It Works

1. **WDT Oscillator**: The internal WDT oscillator is used to track time. It operates independently of the main system clock, consuming minimal power.
2. **Deep Sleep**: The microcontroller enters deep sleep mode between tasks, further reducing power consumption.
3. **Calibration**: Every 15 minutes, a timer is used to measure and calibrate the WDT tick count. This ensures accurate timekeeping despite the WDT oscillator's temperature dependency.

## Usage

1. Compile the code using the provided Makefile.
2. Flash the compiled firmware to an ATtiny13 microcontroller.
3. Connect an output device to the appropriate pin (PB4 in the example).
4. The microcontroller will toggle the output at the specified intervals.

## Fine Tuning

For fine tuning, change the `OCR0A = 248;` value or increase/decrease the `_delay_ms` inside the quarter loop.

## Notes

- The WDT oscillator's accuracy depends on temperature, so calibration is essential for precise timing.
- This implementation is ideal for applications requiring ultra-low power consumption and periodic task execution.

## Files

- `main.c`: The main implementation of the 24-hour timer with WDT calibration.
- `main_1s_blink.c`: A simple example to blink an LED every second.
- `main_10Hz_timer.c`: Code used to calibrate the timer to get exactly a 10 Hz signal.et exactly a 10 Hz signal.
- `Makefile`: Build and flash instructions for the ATtiny13.



