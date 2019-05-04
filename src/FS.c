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

extern uint8_t data_block[BLOCK_SIZE];


uint32_t FS_format() {
    struct fs_header * header = data_block;

    header->n_files = 0;
    header->files[0].first_block = 0x1;
    header->files[0].last_block = 0x1;
    header->files[0].last_byte = 0;

    SD_WRITE_BLOCK(0x00000000, header);

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
// all audio should be 8 bit single channel, 8000 Hz


// TODO: check endianness
uint32_t FS_upload_file(){
    struct fs_header * header = data_block;
    struct file_header * last_file;
    struct file_header * next_file;
    uint32_t bytes_left;
    uint32_t inc_bytes;
    uint32_t first_block;
    uint32_t last_block;
    uint16_t last_byte;

    SD_READ_SINGLE_BLOCK(0x0, data_block);
    last_file = &header->files[header->n_files - 1];
    next_file = &header->files[header->n_files];

    // check if there's room for another file
    if (header->n_files >= MAX_FILES) {
        UART0_write_byte(header->n_files);
        // UART0_write_byte(FILE_REJECTED);
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
        UART0_write_byte(FILE_REJECTED);
        return FILE_REJECTED;
    } else {
        UART0_write_byte(FILE_ACCEPTED);
    }

    // TODO: somewhere around here is an off by 1 error
    first_block = last_file->last_block + 1;
    last_block = first_block + (inc_bytes / BLOCK_SIZE);
    last_byte = inc_bytes % BLOCK_SIZE;
    // // add the last not full block since integer division will skip it
    // if (last_byte != 0) {
    //     last_block += 1;
    // } else {
    //     last_byte = 511;
    // }

    // add a new file
    next_file->first_block = first_block;
    next_file->last_block = last_block;
    next_file->last_byte = last_byte;

    header->n_files += 1;

    // save header to SD card
    SD_WRITE_BLOCK(0x0, data_block);

    // write whole file to SD card
    for (; first_block < last_block; first_block++) {
        UART0_write_byte(FILE_ACCEPTED);
        // UART0_write_byte(last_block - first_block);
        FS_upload_block(data_block, first_block);
    }

    UART0_write_byte(TRANSFER_COMPLETE);
    return TRANSFER_COMPLETE;
}
