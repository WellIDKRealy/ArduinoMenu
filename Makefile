MCU = atmega328p
F_CPU = 16000000UL
ARDUINO=ARDUINO_NANO

FORMAT = ihex

DEVICE = /dev/ttyUSB0

SRC = $(wildcard *.c)

CFLAGS = -O2 -std=gnu99
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -ffunction-sections -fdata-sections -fno-fat-lto-objects
CFLAGS += -Wall -Wstrict-prototypes
CFLAGS += -static -flto
CFLAGS += -D GRAPHICMODE

GUIX_INVOKE = guix time-machine -C channels.scm -- shell -m manifest.scm --pure -- make

build:
	@${GUIX_INVOKE} build_real

flash:
	@${GUIX_INVOKE} flash_real

build_real: main.hex main.elf
	@avr-size -A main.elf

flash_real: main.hex
	avrdude -p ${MCU} -c arduino -P ${DEVICE} -b 115200 -U flash:w:main.hex:i

main.elf: ${SRC}
	avr-gcc ${SRC} -o main.elf -mmcu=${MCU} -DF_CPU=${F_CPU} -D${ARDUINO} ${CFLAGS} ${LFLAGS}

main.hex: main.elf
	avr-objcopy -O ihex -R .eeprom main.elf main.hex

clean:
	rm main.hex main.elf

.PHONY: flash flash_real clean build build_real
