#include <avr/io.h>

#include "UART.h"


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

void timer_1A_init() {
    // disconnected
    TCCR1A &= ~_BV(COM1A0);
    TCCR1A |= _BV(COM1A1);

    // prescaled (clock / 1024)
    TCCR1B |= _BV(CS10);
    TCCR1B &= ~_BV(CS11);
    TCCR1B &= ~_BV(CS12);

    // phase correct pwm, TOP = OCR1A
    TCCR1A |= _BV(WGM10);
    TCCR1A |= _BV(WGM11);
    TCCR1B &= ~_BV(WGM12);
    TCCR1B |= _BV(WGM13);

    // enable interrupt
    TIMSK1 |= _BV(OCIE1A);

    // max value
    OCR1A = 5000;
}

void pwm_0A_run() {
    while (1) {
        char a;
        // OCR0A = UDR0;
        a = UART0_read_byte();
        OCR0A = a;
        // UDR0 = OCR0A;
        UART0_write_byte(a);
        // _delay_us(69);
    }
}