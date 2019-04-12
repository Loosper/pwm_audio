#include <avr/io.h>
#include <stdlib.h>         // For malloc() and free()
#include <avr/pgmspace.h>   // For storing CMD sequences in Flash memory
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "SD.h"
#include "UART.h"

// Dummy bytes needed to prepare SD card
volatile uint8_t dummy_mosi_data[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
// Software reset
volatile uint8_t rst_mosi_data[6] = {SDBEG + CMD0, 0x00, 0x00, 0x00, 0x00, 0x95};
// Check voltage range (only for SDC V2)
volatile uint8_t chk_mosi_data[6] = {SDBEG + CMD8, 0x00, 0x00, 0x01, 0xAA, 0x87};
// Initiate initialization process
volatile uint8_t init_mosi_data[6] = {SDBEG + CMD1, 0x00, 0x00, 0x00, 0x00, 0x00};

int send_mosi_data(volatile uint8_t *mosi_data){
    for(int i = 0 ; i < MSG_LENGTH ; ++ i){
        SPDR = mosi_data[i];  // Set the SPI data register
        while(!(SPSR & (1<<SPIF)));              // Wait for data to be sent

        // It usually takes a few clock cycles
        // for the SD card to respond. During 
        // that time the MISO line is held high
        // aka recieving 0xFF on the input
        if (SPDR != 0xFF){         
            // Write the recieved data from the SPI
            // to the UART. The first byte is a
            // status message indicating if the 
            // command was recieved succesfully.
            // 0x00 and 0x01 are generally a good sign.
            // The rest of the message bytes are accordig 
            // to the command
            UART0_write_byte(SPDR);
        }
    }

    return 1;
}


int main(void){
    sei();                                          // Enable interrupts
     
    UART0_init();                                   // Init UART with BAUD rate
                                                    // specified in UART.h
    UART0_TX_enable();
    UART0_RX_enable();
    
    _delay_ms(3000);

    DDRB |= (1 << DDB2) | (1 << DDB3) | (1 << DDB5); // SCK, MOSI and SS as outputs
    DDRB &= ~(1 << DDB4);                            // MISO as input
    PORTB |= (1 << DDB4);                            // Enable pull-up resistor
                                                     // MOSI pin must be driven HIGH(1) 
                                                     // for the communication to occur

    SPCR |= (1 << MSTR);                 // Set as master
    SPCR |= (1 << SPR0) | (1 << SPR1);   // Set to highest division factor (Clock = 125kHz)
                                         // Prior to initialization the clock frequency
                                         // should be between 100kHz and 400kHz. After
                                         // that it can be set to a desired one
    SPCR &= ~((1 << CPOL) | (1 <<CPHA)); // Set SPI mode to (0;0)
    // SPCR |= (1 << SPIE)                  // Enable SPI interrupt
    SPCR |= (1 << SPE);                  // Enable SPI 

    PORTB &= ~(1 << PORTB2);             // Set SS line LOW(0) to begin communication
   
        
    send_mosi_data(dummy_mosi_data);           // Send a total of 12 dummy bytes
    send_mosi_data(dummy_mosi_data);


    send_mosi_data(rst_mosi_data);
    send_mosi_data(dummy_mosi_data);

    send_mosi_data(chk_mosi_data);
    send_mosi_data(dummy_mosi_data);
    while(1);
}

ISR (USART_RX_vect){
    uint8_t cmd = UART0_read_byte();
    volatile uint8_t *mosi_data;
    switch(cmd){
        case 0: mosi_data = rst_mosi_data; break;
        case 8: mosi_data = chk_mosi_data; break;
        default: mosi_data = dummy_mosi_data; break;
    }
}
