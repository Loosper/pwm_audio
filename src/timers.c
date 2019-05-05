#include <avr/io.h>

#include "UART.h"


void timer_0A_init() {
    // set PD6 (also OC0A) as output (arduino pin 6)
    // DDRD |= _BV(DDD6);

    // CS2 CS1 CS0  timer frequency
    //  0   0   0   disabled
    //  0   0   1   no prescaler;
    //  0   1   0   /8; other values: 64, 256, 1024 by adding 1
    TCCR0B &= ~_BV(CS02);
    TCCR0B |=  _BV(CS01);
    TCCR0B &= ~_BV(CS00);

    // WGM2 WGM1 WGM0 toggle settings
    //  0    0    0   timer disabled
    //  0    1    0   clear tixmer on compare (CTC), TOP = OCR0A
    //  1    1    0   phase correct, TOP = 0xFF
    TCCR0B &= ~_BV(WGM02);
    TCCR0A |=  _BV(WGM01);
    TCCR0A &= ~_BV(WGM00);

    // WARNING: COM registers have different meaning on different modes
    // COM1 COM0 port control
    //   0   0   OC0 disabled
    //   0   1   toggle on compare
    //   1   0   clear on compare
    TCCR0A &= ~_BV(COM0A1);
    TCCR0A &= ~_BV(COM0A0);

    // enable intrrupt on compare match (TIMER0_COMPA_vect )
    TIMSK0 |= _BV(OCIE0A);

    // count to
    OCR0A = 250;
}

void timer_1A_init() {
    // set PB1 (also OC1A) as output (arduino pin 9)
    DDRB |= _BV(DDB1);

    // CS2 CS1 CS0  timer frequency
    //  0   0   0   disabled
    //  0   0   1   no prescaler;
    //  0   1   0   /8; other values: 64, 256, 1024 by adding 1
    TCCR1B &= ~_BV(CS12);
    TCCR1B &= ~_BV(CS11);
    TCCR1B |=  _BV(CS10);

    // WGM13 WGM12 WGM11 WGM10 toggle settings
    //   0     0     0     0   normal
    //   1     0     0     1   phase and frequency correct, TOP = OCR1 (9)
    //   1     0     1     1   phase correct, TOP = OCR1 (11)
    //   1     1     1     1   fast pwm, TOP = OCR1 (15)
    TCCR1B |=  _BV(WGM13);
    TCCR1B &= ~_BV(WGM12);
    TCCR1A |=  _BV(WGM11);
    TCCR1A |=  _BV(WGM10);

    // COM1 COM0 port control
    //   0   0   disconnected (generic pin)
    //   0   1   OC1A connected only for WGM 9 or 11. Disconnected oterwise
    //   1   0   non inverting mode
    TCCR1A &= ~_BV(COM1A1);
    TCCR1A |=  _BV(COM1A0);

    // enable interrupt on compare match (TIMER1_COMPA_vect)
    // TIMSK1 |= _BV(OCIE1A);

    // count to
    OCR1A = 0;
}

void toggle_pwm() {
    DDRB ^= _BV(DDB1);
}

void pwm_run() {
    while (1) {
        char a;
        // OCR1A = UDR0;
        a = UART0_read_byte();
        OCR1A = a;
        // UDR0 = OCR0A;
        UART0_write_byte(a);
        // _delay_us(69);
    }
}