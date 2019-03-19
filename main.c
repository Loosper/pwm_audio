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

// UART recieve interrupt handler
ISR(USART_RX_vect) {
    OCR0A = UDR0;
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

    // while (1) {
    //     OCR0A++;
    //     _delay_ms(10);
    // }

    // will jump to start otherwise
    while (1);
}
