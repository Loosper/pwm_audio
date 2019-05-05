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


extern uint8_t data_block[BLOCK_SIZE];
uint8_t extra_block[BLOCK_SIZE];
uint8_t * current_block;
extern struct file_system fs;

volatile uint16_t idx = 0;
volatile int8_t reload = 0;

// PB5 is connected to blue led on arduino nano

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
        case 0xAB:              resp = FS_info();              break;
        case 0xAD:              resp = 0x21;                   break;
    }

    UART0_write_bytes(&resp, sizeof(resp));
}

// will fire when a sample needs changing
ISR(TIMER0_COMPA_vect) {
    if (idx != BLOCK_SIZE) {
        OCR1A = current_block[idx];
        idx += 1;
    }
}

int main(void) {
    UART0_init();
    UART0_TX_enable();
    UART0_RX_enable();

    SPI_init();
    SD_init();
    FS_init();

    timer_0A_init();
    timer_1A_init();

    // enable interrupts
    sei();

    current_block = extra_block;
    while (1) {
        if (idx == BLOCK_SIZE) {
            idx = 0;

            if (FS_read_block(current_block) == ERROR) {
                FS_reset_file();
            }

            if (current_block == data_block) {
                current_block = extra_block;
            } else {
                current_block = data_block;
            }
        }
    }
}
