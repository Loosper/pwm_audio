#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/setbaud.h>

#include "library.h"


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


ISR(USART_RX_vect) {
    led_blue_flip();
    char broken = UDR0;

    OCR0A = broken;
    led_blue_flip();
}

int main (void) {
    wdt_disable();
    UART0_init();
    pwm_0A_init();

    UART0_TX_enable();
    UART0_RX_enable();

    led_blue_on();

    // enable interrupts
    sei();

    while (1) {
        OCR0A++;
        _delay_ms(10);
    }
    // char a = 55;
    // while (1) {
    //     led_blue_flip();
    //     OCR0A = a;
    //     a = UART0_read_char();
    //     UART0_write_char(a);
    //     _delay_ms(500);
    // }


    // while (1) {
    //     UART0_write_char('1');
    //     UART0_write_char('2');
    // }

    // will jump to start otherwise
    while (1);
}
