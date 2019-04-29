#include <stdint.h>

// #define MYUBRR FOSC/16/MYBAUD -1            // Corresponding UBBR value

void UART0_init();
void UART0_RX_enable();
void UART0_TX_enable();
void UART0_write_byte(uint8_t);
void UART0_write_bytes(uint8_t*, uint8_t);
char UART0_read_byte();
void UART0_read_bytes(uint8_t *, uint8_t);
