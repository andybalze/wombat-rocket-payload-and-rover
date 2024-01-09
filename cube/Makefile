.PHONY: all compile size fuse flash clean

# Below is an example makefile for AVR. We'll need to modify at least build/main.out for our needs.
# (I just compile the whole program at once. If compile times get long, we can change that.)

all: compile fuse flash

compile: build/main.hex size

build/main.hex: build/main.out
	avr-objcopy -j .text -j .data -O ihex build/main.out build/main.hex

build/main.out: main.cpp timers.cpp timers.h io.cpp io.h button.cpp button.h eeprom.cpp eeprom.h util.cpp util.h time.cpp time.h data.cpp data.h bright.cpp bright.h
	avr-gcc main.cpp timers.cpp io.cpp button.cpp eeprom.cpp util.cpp time.cpp data.cpp bright.cpp -mmcu=attiny84a -o build/main.out -Os


size: build/main.hex
	avr-size build/main.hex

fuse:
	avrdude -p t84 -c usbtiny -U lfuse:w:0xFF:m -U hfuse:w:0xDF:m -U efuse:w:0xFF:m -U lock:w:0xFF:m

flash: build/main.hex
	avrdude -p t84 -c usbtiny -U flash:w:build/main.hex:i
	
clean:
	rm -f build/main.hex
	rm -f build/main.out