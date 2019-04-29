#include <stdint.h>

#define F_CPU 16000000                      // Clock Speed
#define BAUD 76800                         // Baud rate
// #define MYUBRR FOSC/16/MYBAUD -1            // Corresponding UBBR value

void UART0_init();
void UART0_RX_enable();
void UART0_TX_enable();
void UART0_write_byte(uint8_t);
void UART0_write_bytes(uint8_t*, uint8_t);
char UART0_read_byte();
