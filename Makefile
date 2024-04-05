MCU = atmega328p
F_CPU = 16000000UL
ARDUINO=ARDUINO_NANO

FORMAT = ihex

DEVICE = /dev/ttyUSB0

CSRC = $(wildcard *.c)
HSRC = $(wildcard *.j)

FLAGS = -mmcu=${MCU} -DF_CPU=${F_CPU} -D${ARDUINO}
FLAGS += -O2 -static -flto
FLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
FLAGS += -ffunction-sections -fdata-sections -fno-fat-lto-objects
FLAGS += -Wall
FLAGS += -D GRAPHICMODE
FLAGS += -lm

CFLAGS = -std=gnu99 -Wstrict-prototypes

GUIX_INVOKE = guix time-machine -C channels.scm -- shell -m manifest.scm --pure -- make

build:
	@${GUIX_INVOKE} build_real

flash:
	@${GUIX_INVOKE} flash_real

build_real: main.hex main.elf
	@avr-size -A main.elf

flash_real: main.hex
	avrdude -p ${MCU} -c arduino -P ${DEVICE} -b 115200 -U flash:w:main.hex:i


main.elf: ${CSRC} ${HSRC}
	avr-gcc -o main.elf ${CSRC} ${FLAGS} ${CFLAGS}

main.hex: main.elf
	avr-objcopy -O ihex -R .eeprom main.elf main.hex

clean:
	rm main.hex main.elf
	rm *.o

.PHONY: flash flash_real clean build build_real
