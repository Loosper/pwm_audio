#include <avr/io.h>
#include <util/setbaud.h>

#include "library.h"


void UART0_init() {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    // double speed if necessary for this baud rate
    #if USE_2X
        UCSR0A |=  _BV(U2X0);
    #else
        UCSR0A &= ~_BV(U2X0);
    #endif

    // 8-bit data
    UCSR0C |=  _BV(UCSZ00);
    UCSR0C |=  _BV(UCSZ01);
    UCSR0B &= ~_BV(UCSZ02);

    // run in async mode is ok
    // don't care for parity bits
    // stop bits can be 1 (default)

    UCSR0B |= _BV(RXCIE0);
}

inline void UART0_RX_enable() {
    UCSR0B |= _BV(RXEN0);
}

inline void UART0_TX_enable() {
    UCSR0B |= _BV(TXEN0);
}

void UART0_write_char(char c) {
    // wait for last transmission
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
}

char UART0_read_char() {
    loop_until_bit_is_set(UCSR0A, RXC0);
    return UDR0;
}

// values: https://sites.google.com/site/qeewiki/books/avr-guide/pwm-on-the-atmega328
void pwm_0A_init() {
    // set PD6 as output
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
    TCCR0A &= ~_BV(WGM01);
    TCCR0B &= ~_BV(WGM02);

    // duty cycle 0
    OCR0A = 0;
}
