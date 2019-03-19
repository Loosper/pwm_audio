all: compile upload

compile main.c library.c:
	avr-gcc -Os -DF_CPU=16000000UL -DBAUD=57600 -mmcu=atmega328p -c main.c -o main.o
	avr-gcc -Os -DF_CPU=16000000UL -DBAUD=57600 -mmcu=atmega328p -c library.c -o lib.o
	avr-gcc -mmcu=atmega328p main.o lib.o -o main
	avr-objcopy -O ihex -R .eeprom main main.hex

upload:
	avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyUSB0 -b 57600 -U flash:w:main.hex
