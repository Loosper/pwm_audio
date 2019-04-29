CFLAGS = -Os -I include -DBAUD=76800 -DF_CPU=16000000UL -mmcu=atmega328p -Wno-incompatible-pointer-types
LDFLAGS = -mmcu=atmega328p
CC = avr-gcc

SRC = $(wildcard src/*.c)
OBJ = $(SRC:%.c=%.o)
DEPS := $(OBJ:.o=.d)

all: compile upload
main: $(OBJ)

-include $(DEPS)
%.o: %.c
	$(CC) $(CFLAGS) -MMD -MF $(patsubst %.o,%.d,$@) -c -o $@ $<

compile: main
	avr-objcopy -O ihex -R .eeprom main main.hex

upload:
	# for uno
	avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:main.hex
	# for nano
	# avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyUSB0 -b 57600 -U flash:w:main.hex

.PHONY: clean
clean:
	rm -f main main.hex $(OBJ) $(DEPS)
