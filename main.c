#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/setbaud.h>

#define BLINK_DELAY_MS 10000

// // watchdog interrupt
// ISR(WDT_vect) {
//     PORTB |= _BV(PORTB5);
//     _delay_ms(BLINK_DELAY_MS);
// }

void HW_UART_init() {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    // enables double speed
    #if USE_2X
        UCSR0A |= _BV(U2X0);
    #else
        UCSR0A &= ~(_BV(U2X0));
    #endif

    // 8-bit data
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
    // enable RX and TX
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);
}

void HW_UART_write_char(char c) {
    // wait for last transmission
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
}

int main (void) {
    HW_UART_init();
    // set pin 5 of PORTB for output
    DDRB |= _BV(DDB5);

    wdt_disable();
//     enable watchdog timer
//     wdt_enable(WDTO_1S);

    while (1) {
        HW_UART_write_char('1');
        HW_UART_write_char('2');
    }

    while(1) {
        // set pin 5 high to turn led on
        PORTB |= _BV(PORTB5);
        _delay_ms(BLINK_DELAY_MS);

        // set pin 5 low to turn led off
        PORTB &= ~_BV(PORTB5);
        _delay_ms(BLINK_DELAY_MS);
    }
}
