// SD file format:
//
//    4 bytes       4 bytes        N bytes
// +-----------+-------------+-------- ... ---+
// | File size |  32 bit CRC |   DATA         |
// +-----------+-------------+-------- ... ---+
//
#include <stdint.h>

#define BEG_ADDRESS 0x00000000
#define HEADER_SIZE 8

uint32_t FS_next_file();          // Move location variables to next file
uint32_t FS_prev_file();          // Move location variables to prexious file
void FS_read_block();         // Read next block from the current file

void FS_init();
uint32_t FS_upload_file();     // Upload a file throught the UART
uint32_t FS_get_last_location();
uint32_t FS_get_file_size();
uint32_t FS_format();
