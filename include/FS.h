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
// this is actually 16 MB but whatever
#define MAX_BLOCKS 32768

#define ERROR -1
#define SUCCESS 0
#define FILE_ACCEPTED 1
#define FILE_REJECTED 0
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

struct file_system {
    uint8_t file_id;
    uint32_t first_block;
    uint32_t cur_block;
    uint32_t last_block;
    uint16_t last_byte;
};


void FS_init();
uint32_t FS_info();
uint32_t FS_format();
uint32_t FS_upload_file();
int8_t FS_next_file();
int8_t FS_prev_file();
void FS_reset_file();
int16_t FS_read_block(uint8_t* buff);
