#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "UART.h"
#include "SPI.h"
#include "SD.h"
#include "FS.h"
#include "timers.h"


// not working
ISR (USART_RX_vect){
    uint8_t cmd = UART0_read_byte();
    uint32_t resp = 0xFFFFFFFF;
    switch(cmd){
        case GO_IDLE_STATE:     resp = SD_GO_IDLE_STATE();     break;
        case SEND_IF_COND:      resp = SD_SEND_IF_COND();      break;
        case APP_SEND_OP_COND:  resp = SD_APP_SEND_OP_COND();  break;
        case READ_SINGLE_BLOCK: resp = SD_GO_IDLE_STATE() ;    break;
        case WRITE_BLOCK:       resp = FS_upload_file() ;      break;
        case 0xAA:              resp = FS_format();            break;
        case 0xAD:              resp = 0x21;                   break;
        // case 0xAB:              resp = FS_get_last_location(); break;
        // case 0xAC:              resp = FS_get_file_size();     break;
    }

    UART0_write_bytes(&resp, sizeof(resp));
}

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

// will fire when a sample needs changing
// ISR(TIMER0_COMPA_vect) {
//     OCR1A = latest;
// }

// // UART recieve
// ISR(USART_RX_vect) {
//     latest = UDR0;
//     // UART0_write_byte(latest);
// }

int main(void) {
    // enable interrupts
    sei();

    UART0_init();
    UART0_TX_enable();
    UART0_RX_enable();

    SPI_init();
    SD_init();

    // timer_0A_init();
    // timer_1A_init();

    // init ready if you will
    // led_blue_on();

    // pwm_run();

    while(1);
}
