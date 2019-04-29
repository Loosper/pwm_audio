// SD file format:
//
// The very first 4 bytes of the SD card should
// indicate the position of the last file.
//
// Due to reading being limited to chunks with
// size BLOCK_SIZE, all sizes and locations
// are saved using BLOCK_SIZE as a unit.
//
//    4 bytes       4 bytes        N bytes
// +-----------+-------------+-------- ... ---+
// | File size |  32 bit CRC |   DATA         |
// +-----------+-------------+-------- ... ---+
//
#include <stdint.h>
#include <stdlib.h>

#include "FS.h"
#include "SD.h"
#include "UART.h"

extern uint8_t * data_block;

// // Little TO Big Endian
// //
// uint32_t ltobe(uint32_t lit){
//     uint32_t b0 = (lit & 0x000000FF) << 24;
//     uint32_t b1 = (lit & 0x0000FF00) << 8;
//     uint32_t b2 = (lit & 0x00FF0000) >> 8;
//     uint32_t b3 = (lit & 0xFF000000) >> 24;

//     return b0 | b1 | b2 | b3;
// }

uint32_t FS_format(){
    struct fs_header * header = data_block;

    header->n_files = 0;
    header->files[0].first_block = 0x1;
    header->files[0].last_block = 0x1;
    header->files[0].last_byte = 0;

    SD_WRITE_BLOCK(0x00000000, data_block);

    return 0x0;
}

static void FS_upload_block(uint8_t* data_block, uint32_t address){
    for (int i = 0; i < BLOCK_SIZE; i++) {
        data_block[i] = UART0_read_byte();
    }

    SD_WRITE_BLOCK(address, data_block);
}

// transfer protocol after initiating upload
// any rejection terminates the transfer
// player       PC
// 1 byte  -->  room for more files (true/false)
// 4 bytes -->  room left
// 4 bytes <--  bytes to send
// 1 byte -->   will fit (true/false)
// loop:
// 1 byte -->   ready for transmission. on value 2 => transfer is ready
// <=512  <--   receive up to 512 bytes


// TODO: check endianness
uint32_t FS_upload_file(){
    struct fs_header * header = data_block;
    struct file_header * last_file;
    struct file_header * next_file;
    uint32_t bytes_left;
    uint32_t inc_bytes;
    uint32_t first_block;
    uint32_t last_block;

    SD_READ_SINGLE_BLOCK(0x0, data_block);
    last_file = &header->files[header->n_files - 1];
    next_file = &header->files[header->n_files];

    // check if there's room for another file
    if (header->n_files >= MAX_FILES) {
        return FILE_REJECTED;
    } else {
        // number of bytes that can be allocated for a new file
        bytes_left = (
            MAX_BLOCKS - last_file->last_block
        ) * BLOCK_SIZE;
        UART0_write_byte(FILE_ACCEPTED);
        UART0_write_bytes(&bytes_left, 4);
    }

    UART0_read_bytes(&inc_bytes, 4);

    // check if new file will fit
    if (inc_bytes > bytes_left) {
        UART0_write_byte(FILE_ACCEPTED);
    } else {
        return FILE_REJECTED;
    }

    // add a new file
    next_file->first_block = last_file->last_block + 1;
    next_file->last_block = next_file->first_block + (inc_bytes / BLOCK_SIZE);
    next_file->last_byte = inc_bytes % BLOCK_SIZE;
    header->n_files += 1;
    // save locally since they will be overwritten in a sec
    first_block = next_file->first_block;
    last_block = next_file->last_block;

    // save header to SD card
    SD_WRITE_BLOCK(0x0, data_block);

    // write whole file to SD card
    for (; first_block <= last_block; first_block++) {
        UART0_write_byte(FILE_ACCEPTED);
        FS_upload_block(data_block, first_block);
    }

    return TRANSFER_COMPLETE;
}
