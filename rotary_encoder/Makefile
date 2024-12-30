#########################################################
# Raw Rotary Encoder
# ########################################################

# MCU and Frequency settings
MCU   = atmega328p
F_CPU = 8000000
BAUD  = 9600

# Main source file
MAIN = main_rotary_encoder.c

# Programmer settings
PROGRAMMER_TYPE = usbasp
PROGRAMMER_ARGS = -b 38400

# Toolchain programs
CC      = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE = avr-size
AVRDUDE = avrdude

# Compilation options
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU)UL -DBAUD=$(BAUD)
CFLAGS += -Os -Wall -Wstrict-prototypes -std=gnu99
CFLAGS += -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,--relax
CFLAGS += -g -ggdb

# Source and target files
TARGET = $(basename $(MAIN))
SRC = $(TARGET).c

# Object files
OBJ = $(SRC:.c=.o)

# Default target
all: $(TARGET).hex

# Build hex file from elf file
%.hex: %.elf
	$(OBJCOPY) -R .eeprom -O ihex $< $@

# Build elf file from source files
%.elf: $(SRC)
	$(CC) $(CFLAGS) $< -o $@

# Optional targets
disassemble: $(TARGET).lst
%.lst: %.elf
	$(OBJDUMP) -S $< > $@

size: $(TARGET).elf
	$(AVRSIZE) -C --mcu=$(MCU) $<

clean:
	rm -f $(TARGET).elf $(TARGET).hex $(OBJ) $(TARGET).lst

# Flash the program
flash: $(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U flash:w:$<

# Fuse settings
LFUSE = 0xE2
HFUSE = 0xD9
EFUSE = 0xFF
FUSE_STRING = -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m

fuses:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) $(FUSE_STRING)

show_fuses:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -nv

set_default_fuses: fuses

