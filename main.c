#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/setbaud.h>
#include <util/delay.h>

#include "UART.h"
#include "SPI.h"
#include "SD.h"


// UART recieve interrupt handler
// ISR(USART_RX_vect) {
//     OCR0A = UDR0;
//     UDR0 = OCR0A;
//     _delay_us(69);
//     // TODO: do this in a loop wihtou interrupts
// }

// not working
// ISR (USART_RX_vect){
//     uint8_t cmd = UART0_read_byte();
//     volatile uint8_t *mosi_data;
//     switch(cmd){
//         case 0: mosi_data = rst_mosi_data; break;
//         case 1: mosi_data = init_mosi_data; break;
//         case 8: mosi_data = chk_mosi_data; break;
//         case 58: mosi_data = ocr_mosi_data; break;
//         case 41: mosi_data = initsdc_mosi_data; break;
//         default: mosi_data = dummy_mosi_data; break;
//     }
//
//     send_mosi_data(mosi_data);
// }

// values: https://sites.google.com/site/qeewiki/books/avr-guide/pwm-on-the-atmega328
void pwm_0A_init() {

    // set PD6 (also OC0A) as output (arduino pin 6)
    DDRD |= _BV(DDD6);

    // set to non-inverting mode
    TCCR0A &= ~_BV(COM0A0); // 1
    TCCR0A |=  _BV(COM0A1); // 0

    // no prescaling (should be 16MHz)
    TCCR0B |=  _BV(CS00);
    TCCR0B &= ~_BV(CS01);
    TCCR0B &= ~_BV(CS02);

    // phase correct, TOP = 0xFF
    TCCR0A |=  _BV(WGM00);
    TCCR0A |=  _BV(WGM01);
    // TCCR0A &= ~_BV(WGM01);
    TCCR0B &= ~_BV(WGM02);

    // duty cycle 0
    OCR0A = 0;
}

// void timer_1A_init() {

// }

int main(void){
    sei();                                          // Enable interrupts

    UART0_init();                                   // Init UART with BAUD rate
                                                    // specified in UART.h
    UART0_TX_enable();
    UART0_RX_enable();

    SPI_init();
    SD_init();

    // enable interrupts
    sei();

    // int i = 0;
    // int inc = 1;

    // while (1) {
    //     if (i >= 255) {
    //         inc = -1;
    //     } else if (i <= 0) {
    //         inc = 1;
    //     }

    //     i += inc;
    //     OCR0A = i;
    //     // _delay_us(69);
    // }

    while (1) {
        // char a;
        // OCR0A = UDR0;
        OCR0A = UART0_read_byte();
        // OCR0A = a;
        // UDR0 = OCR0A;
        UART0_write_byte(OCR0A);
        // _delay_us(69);
    }

    while(1);
}
