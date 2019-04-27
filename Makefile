CFLAGS = -Os -DF_CPU=16000000UL -DBAUD=9600 -mmcu=atmega328p
LDFLAGS = -mmcu=atmega328p
CC = avr-gcc

all: compile upload
main: main.o UART.o SD.o SPI.o timers.c

compile: main
	avr-objcopy -O ihex -R .eeprom main main.hex

upload:
	# for uno
	avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:main.hex
	# for nano
	# avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyUSB0 -b 57600 -U flash:w:main.hex

clean:
	rm -f main main.hex *.o
