#########################################################
MCU   = t13
CMCU = attiny13a
F_CPU = 1200000

## This is where your main() routine lives 
#MAIN = main_1s_blink.c
#MAIN = main_10Hz_timer.c
MAIN = main.c

#########################################################
##########    Programmer Defaults              ##########
#########################################################
PROGRAMMER_TYPE = usbasp
#PROGRAMMER_TYPE = -P /dev/ttyUSB0 
# extra arguments to avrdude: baud rate, chip type, -F flag, etc.
PROGRAMMER_ARGS = -b 38400 
#########################################################
## Defined programs / locations
#TOOLDIR = ../avr8-gnu-toolchain/bin/

TOOLDIR = 
CC      = avr-gcc
AS      = $(TOOLDIR)avr-as
LD      = $(TOOLDIR)avr-ld
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE = avr-size
#AVRDUDE = $(TOOLDIR)avrdude
AVRDUDE = avrdude

#########################################################
## Compilation options, type man avr-gcc if you're curious.
CFLAGS = -mmcu=$(CMCU) -DF_CPU=$(F_CPU)UL $(MYCDEF)
CFLAGS += -Os -I. -I$(EXTRA_SOURCE_DIR)
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums 
CFLAGS += -Wall -Wstrict-prototypes
CFLAGS += -g -ggdb
CFLAGS += -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,--relax
CFLAGS += -std=gnu99

## Lump target and extra source files together
TARGET = $(strip $(basename $(MAIN)))
SRC = $(TARGET).c

## List of all header files
HEADERS = $(SRC:.c=.h) 

## For every .c file, compile an .o object file
OBJ = $(SRC:.c=.o) 

## Generic Makefile targets.  (Only .hex file is necessary)
all: $(TARGET).hex

%.hex: %.elf
	$(OBJCOPY) -R .eeprom -O ihex $< $@

%.elf: $(SRC)
	$(CC) $(CFLAGS) $(SRC) --output $@ 

%.eeprom: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@ 

debug:
	@echo
	@echo "Source files:"   $(SRC)
	@echo "MCU, F_CPU, BAUD:"  $(MCU), $(F_CPU), $(BAUD)
	@echo	

%.lst: %.elf
	$(OBJDUMP) -S $< > $@

# Optionally show how big the resulting program is 
size:  $(TARGET).elf
	$(AVRSIZE) -C --mcu=$(MCU) $(TARGET).elf

clean:
	rm -f $(TARGET).elf $(TARGET).hex $(TARGET).obj \
	$(TARGET).o $(TARGET).d $(TARGET).eep $(TARGET).lst \
	$(TARGET).lss $(TARGET).sym $(TARGET).map $(TARGET)~ \
	$(TARGET).eeprom

squeaky_clean:
	rm -f *.elf *.hex *.obj *.o *.d *.eep *.lst *.lss *.sym *.map *~

##########
##########              Programmer-specific details       

flash: $(TARGET).hex 
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U flash:w:$<

#############################################################
########       Fuse settings and suitable defaults ##########
# ATtiny13
LFUSE = 0x6A
HFUSE = 0xFF
LOCKBIT = 0xFF


## Generic 
FUSE_STRING = -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U lock:w:$(LOCKBIT):m 

fuses: 
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) \
	           $(PROGRAMMER_ARGS) $(FUSE_STRING)
show_fuses:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -nv	

## Called with no extra definitions, sets to defaults
set_default_fuses:  FUSE_STRING = -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U lock:w:$(LOCKBIT):m 
set_default_fuses:  fuses


