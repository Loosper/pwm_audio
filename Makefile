all: compile upload

compile:
	# Add malloc restirctions with __malloc_heap_end and __maloc_heap_start
	avr-gcc -Os -DF_CPU=16000000UL -DBAUD=57600 -mmcu=atmega328p -c main.c -o main.o
	avr-gcc -Os -DF_CPU=16000000UL -DBAUD=57600 -mmcu=atmega328p -c UART.c -o UART.o
	avr-gcc -mmcu=atmega328p main.o UART.o -o main
	avr-objcopy -O ihex -R .eeprom main main.hex

upload:
	avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:main.hex
