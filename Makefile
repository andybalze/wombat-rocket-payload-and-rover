.PHONY: all rover_all rover_compile rover_size rover_fuse rover_flash cube_all cube_compile cube_size cube_fuse cube_flash trx_all trx_compile trx_size trx_fuse trx_flash

# ============ Compile everything ==========

all: rover_compile cube_compile trx_compile

# ============ Rover =============

rover_all: rover_compile rover_fuse rover_flash

rover_compile: build/rover.hex rover_size

build/rover.hex: build/rover.out
	avr-objcopy -j .text -j .data -O ihex build/rover.out build/rover.hex

build/rover.out: rover/main.c
	avr-gcc rover/main.c -mmcu=atmega328p -o build/rover.out

rover_size: build/rover.hex
	avr-size build/rover.hex

rover_fuse:
# to be determined by the fuses we need (avrdude command)
# example: avrdude -p m328p -c usbtiny -U lfuse:w:0xFF:m -U hfuse:w:0xDF:m -U efuse:w:0xFF:m -U lock:w:0xFF:m

rover_flash: build/rover.hex
	avrdude -p m328p -c usbtiny -U flash:w:build/rover.hex:i


# ============ Data Cube (standalone) =============

cube_all: cube_compile cube_fuse cube_flash

cube_compile: build/cube.hex cube_size

build/cube.hex: build/cube.out
	avr-objcopy -j .text -j .data -O ihex build/cube.out build/cube.hex

build/cube.out: cube/standalone/main.c
	avr-gcc cube/standalone/main.c -mmcu=atmega328p -o build/cube.out

cube_size: build/cube.hex
	avr-size build/cube.hex

cube_fuse:
# to be determined by the fuses we need (avrdude command)
# example: avrdude -p m328p -c usbtiny -U lfuse:w:0xFF:m -U hfuse:w:0xDF:m -U efuse:w:0xFF:m -U lock:w:0xFF:m

cube_flash: build/cube.hex
	avrdude -p m328p -c usbtiny -U flash:w:build/cube.hex:i

# ============ Data Cube (rover's transceiver) =============

trx_all: trx_compile trx_fuse trx_flash

trx_compile: build/trx.hex trx_size

build/trx.hex: build/trx.out
	avr-objcopy -j .text -j .data -O ihex build/trx.out build/trx.hex

build/trx.out: cube/rover_trx/main.c
	avr-gcc cube/rover_trx/main.c -mmcu=atmega328p -o build/trx.out

trx_size: build/trx.hex
	avr-size build/trx.hex

trx_fuse:
# to be determined by the fuses we need (avrdude command)
# example: avrdude -p m328p -c usbtiny -U lfuse:w:0xFF:m -U hfuse:w:0xDF:m -U efuse:w:0xFF:m -U lock:w:0xFF:m

trx_flash: build/trx.hex
	avrdude -p m328p -c usbtiny -U flash:w:build/trx.hex:i

# =============== General ========================
	
clean:
	rm -f build/rover.hex
	rm -f build/rover.out
	rm -f build/cube.hex
	rm -f build/cube.out
	rm -f build/trx.hex
	rm -f build/trx.out
