# PWM audio
bare metal arudino audio player with PWM

# Setup
`sudo apt install avr-libc avrdude avr-gcc`

# Notes
PB5 is connected to blue led on arduino nano  
convert file into compatible format: `ffmpeg -i ./song.m4a -ac 1  -acodec pcm_u8 -t 00:00:10 -ar 8000 file.wav`  

## Endianness
The AVR architecture is 8 bit, therefore endianness is meaningless. However, multibyte structures are intrepretted as little endian.  

## Glossary
_BV - Bit Value
wdt - WatchDog Timer

## Avr-libc paths
[avr-libc manual](http://nongnu.org/avr-libc/user-manual/modules.html)  
[arduino nano pinout](http://christianto.tjahyadi.com/wp-content/uploads/2014/11/nano.jpg)  
[atmega 328p datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf)  
[(kind of bad) timers quickref](https://sites.google.com/site/qeewiki/books/avr-guide/pwm-on-the-atmega328)  

[SPI intro](http://www.dejazzer.com/ee379/lecture_notes/lec12_sd_card.pdf)  
[SD card info](http://elm-chan.org/docs/mmc/mmc_e.html)  
[SD card guide](https://openlabpro.com/guide/interfacing-microcontrollers-with-sd-card/)  

`/usr/lib/avr`  
`/usr/lib/avr/include/avr/[iom328p.h]` - definitions for different microcontrollers  

## Notes on USART
* synchronous seems to be called SPI
* async is 'simpler'
