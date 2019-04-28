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

uint32_t FILE_BEG;            // Beggining of the current file
uint32_t FILE_SIZE;           // Size of the current file
uint32_t CUR_LOC;             // Location in the current file (in BLOCK_SIZE)
uint32_t LAST_LOC;            // Location of last file

void FS_init(){
    uint8_t* data_block = malloc(sizeof(uint8_t) * BLOCK_SIZE);
    SD_READ_SINGLE_BLOCK(BEG_ADDRESS, data_block);      // Read the very first block
                                                        // of data ton the SD card
    LAST_LOC = ((uint32_t*) data_block)[0];
    FILE_SIZE = ((uint32_t*) data_block)[1];
    FILE_BEG = sizeof(uint32_t);
    CUR_LOC = FILE_BEG;


}

void FS_next_file(){
    uint8_t* data_block = malloc(sizeof(uint8_t) * BLOCK_SIZE);

    FILE_BEG = FILE_BEG + FILE_SIZE;
    CUR_LOC = FILE_BEG;

    SD_READ_SINGLE_BLOCK(CUR_LOC, data_block);
    FILE_SIZE = ((uint32_t*) data_block)[0];
    free(data_block);
}

void FS_prev_file(){
    uint8_t* data_block = malloc(sizeof(uint8_t) * BLOCK_SIZE);

    FILE_BEG = FILE_BEG - FILE_SIZE;
    if(FILE_BEG < 0) FILE_BEG = 0;
    CUR_LOC = FILE_BEG;

    SD_READ_SINGLE_BLOCK(FILE_BEG, data_block);
    FILE_SIZE = ((uint32_t*) data_block)[0];
    free(data_block);
}

void FS_read_block(uint8_t* data_block){
    SD_READ_SINGLE_BLOCK(CUR_LOC, data_block);
    CUR_LOC += 1;
}

uint8_t FS_upload_block(uint8_t* data_block, uint32_t address){
    for(int i = 0 ; i < BLOCK_SIZE ; ++ i){
        data_block[i] = UART0_read_byte();
        // UART0_write_byte(data_block[i]);
    }

    SD_WRITE_BLOCK(address, data_block);

    return 0x00;
}

uint8_t FS_upload_file(){
    uint8_t* data_block = malloc(sizeof(uint8_t) * BLOCK_SIZE);

    UART0_write_byte(LAST_LOC);
    UART0_write_byte(LAST_LOC >> 8);
    UART0_write_byte(LAST_LOC >> 16);
    UART0_write_byte(LAST_LOC >> 24);

    FS_upload_block(data_block, LAST_LOC);
    LAST_LOC += 1;

    uint32_t f_size = ((uint32_t*) data_block)[0];
    uint32_t f_crc = ((uint32_t*) data_block)[1];

    UART0_write_byte(f_crc);
    UART0_write_byte(f_crc >> 8);
    UART0_write_byte(f_crc >> 16);
    UART0_write_byte(f_crc >> 24);

    for(int i = BLOCK_SIZE ; i < f_size + HEADER_SIZE ; i += BLOCK_SIZE){
        FS_upload_block(data_block, LAST_LOC);
        UART0_write_byte(0x00);

        LAST_LOC += 1;
    }

    ((uint32_t*) data_block)[0] = LAST_LOC;       // Update last location
    SD_WRITE_BLOCK(BEG_ADDRESS, data_block);

    free(data_block);

    return 0xEF;
}

