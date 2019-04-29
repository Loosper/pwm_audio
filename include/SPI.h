#include <stdint.h>
#include <avr/io.h>

void SPI_init();
void SPI_write_byte(uint8_t);
uint8_t SPI_read_byte();
void SPI_set_low_freq();
void SPI_set_high_freq();
