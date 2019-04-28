// SD file format:
//
//    4 bytes       4 bytes        N bytes
// +-----------+-------------+-------- ... ---+
// | File size |  32 bit CRC |   DATA         |
// +-----------+-------------+-------- ... ---+
//
#include <stdint.h>

#define BEG_ADDRESS 0x00000000
#define HEADER_SIZE 64

void FS_next_file();          // Move location variables to next file
void FS_prev_file();          // Move location variables to prexious file
void FS_read_block();         // Read next block from the current file

uint8_t FS_upload_file();     // Upload a file throught the UART

