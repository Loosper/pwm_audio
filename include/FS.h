// SD file format:
//
//    4 bytes       4 bytes        N bytes
// +-----------+-------------+-------- ... ---+
// | File size |  32 bit CRC |   DATA         |
// +-----------+-------------+-------- ... ---+
//
#include <stdint.h>

#define BEG_ADDRESS 0x00000000
// #define HEADER_SIZE 8
// limits for a 4 GB SD card with only the first block as a header
#define MAX_FILES 51
#define MAX_BLOCKS 32768

#define FILE_ACCEPTED 0
#define FILE_REJECTED 1
#define TRANSFER_COMPLETE 2

struct __attribute__((__packed__)) file_header {
    uint32_t first_block;
    uint32_t last_block;
    uint16_t last_byte;
};

struct __attribute__((__packed__)) fs_header {
    uint8_t n_files;
    struct file_header files[MAX_FILES];
};

uint32_t FS_next_file();          // Move location variables to next file
uint32_t FS_prev_file();          // Move location variables to prexious file
void FS_read_block();         // Read next block from the current file

void FS_init();
uint32_t FS_upload_file();     // Upload a file throught the UART
uint32_t FS_get_last_location();
uint32_t FS_get_file_size();
uint32_t FS_format();
