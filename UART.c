#include <avr/io.h>
#include <stdint.h>
#include <util/setbaud.h>

#include "UART.h"


void UART0_init() {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    #if USE_2X
        #warning "2X is needed"
    #endif
    // // double speed if necessary for this baud rate
    // #if USE_2X
    //     UCSR0A |=  _BV(U2X0);
    // #else
    //     UCSR0A &= ~_BV(U2X0);
    // #endif

    // 8-bit data
    UCSR0C |=  _BV(UCSZ00) | _BV(UCSZ01);
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

void UART0_write_byte(uint8_t b) {
    loop_until_bit_is_set(UCSR0A, UDRE0); // Wait until buffer is empty
    // Equivalent to while ( !(UCSR0A & (1 << UDRE0)) )
    UDR0 = b;
}

char UART0_read_byte() {
    loop_until_bit_is_set(UCSR0A, RXC0); // Wait until data is recieved
    // Equivalent to while ( !(UCSR0A & (1 << RXC0)) )
    return UDR0;
}
