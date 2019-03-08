# PWM audio
bare metal arudino audio player with PWM

# Setup
`sudo apt install avr-libc avrdude avr-gcc`

# Notes
## Glossary
_BV - Bit Value
wdt - WatchDog Timer

## Avr-libc paths
[avr-libc manual](http://nongnu.org/avr-libc/user-manual/modules.html)  
`/usr/lib/avr`  
`/usr/lib/avr/include/avr/[iom328p.h]` - definitions for different microcontrollers  

## Notes on USART
* synchronous seems to be called SPI
* async is 'simpler'
