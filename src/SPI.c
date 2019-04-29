#include <avr/io.h>
#include <stdint.h>

void SPI_init(){
    DDRB |= (1 << DDB2) | (1 << DDB3) | (1 << DDB5); // SCK, MOSI and SS as outputs
    DDRB &= ~(1 << DDB4);                            // MISO as input
    PORTB |= (1 << DDB4);                            // Enable pull-up resistor
                                                     // MOSI pin must be driven HIGH(1)
                                                     // for the communication to occur

    SPCR |= (1 << MSTR);                 // Set as master
    SPCR |= (0 << SPR0) | (1 << SPR1);   // Set to highest division factor (Clock = 125kHz)
                                         // Prior to initialization the clock frequency
                                         // should be between 100kHz and 400kHz. After
                                         // that it can be set to a desired one
    SPCR &= ~((1 << CPOL) | (1 <<CPHA)); // Set SPI mode to (0;0)
    // SPCR |= (1 << SPIE)                  // Enable SPI interrupt
    SPCR |= (1 << SPE);                  // Enable SPI

    PORTB &= ~(1 << PORTB2);             // Set SS line LOW(0) to begin communication
}

void SPI_set_low_freq(){
    SPCR |= (0 << SPR0) | (1 << SPR1);   // Set to highest division factor (Clock = 125kHz)
}

void SPI_set_high_freq(){
    SPCR |= (0 << SPR0);   // Set to highest division factor (Clock = 125kHz)
}
void SPI_write_byte(uint8_t byte){
    SPDR = byte;
    while(!(SPSR & (1 << SPIF)));  // Wait until all data is sent
}

uint8_t SPI_read_byte(){
    SPDR = 0xFF;                   // Write a dummy byte to the SPI register
                                   // to generate a clock signal for the slave

    while(!(SPSR & (1 << SPIF)));  // Wait until all data is received
    return SPDR;
}
