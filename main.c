#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/setbaud.h>
#include <util/delay.h>

#include "UART.h"
#include "SPI.h"
#include "SD.h"
#include "timers.h"

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

// built in led control for debugging
void led_blue_on() {
    // set pin 5 high to turn led on
    PORTB |= _BV(PORTB5);
}

void led_blue_off() {
    // set pin 5 high to turn led off
    PORTB &= ~_BV(PORTB5);
}

void led_blue_flip() {
    PORTB ^= _BV(PORTB5);
}

volatile char latest = 0;

// timer 0 match
ISR(TIMER0_COMPA_vect) {
    OCR1A = latest;
    led_blue_flip();
}

// UART recieve
ISR(USART_RX_vect) {
    latest = UDR0;
    // UART0_write_byte(latest);
    UDR0 = latest;
}

int main(void) {
    UART0_init();
    UART0_TX_enable();
    UART0_RX_enable();

    // SPI_init();
    // SD_init();

    timer_0A_init();
    timer_1A_init();

    // init ready if you will
    led_blue_on();
    // enable interrupts
    sei();

    // pwm_run();

    while(1);
}
