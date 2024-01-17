.PHONY: all compile size fuse flash clean

all: compile fuse flash

compile: build/main.hex size

build/main.hex: build/main.out
	avr-objcopy -j .text -j .data -O ihex build/main.out build/main.hex

build/main.out: main.c
	avr-gcc main.c -mmcu=attiny84a -o build/main.out

size: build/main.hex
	avr-size build/main.hex

fuse:
	avrdude -p t84 -c usbtiny -U lfuse:w:0xFF:m -U hfuse:w:0xDF:m -U efuse:w:0xFF:m -U lock:w:0xFF:m

flash: build/main.hex
	avrdude -p t84 -c usbtiny -U flash:w:build/main.hex:i
	
clean:
	rm -f build/main.hex
	rm -f build/main.out