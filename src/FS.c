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

// Little TO Big Endian
//
uint32_t ltobe(uint32_t lit){
    uint32_t b0 = (lit & 0x000000FF) << 24;
    uint32_t b1 = (lit & 0x0000FF00) << 8;
    uint32_t b2 = (lit & 0x00FF0000) >> 8;
    uint32_t b3 = (lit & 0xFF000000) >> 24;

    return b0 | b1 | b2 | b3;
}

void FS_init(){
    uint8_t* data_block = malloc(sizeof(uint8_t) * BLOCK_SIZE);
    SD_READ_SINGLE_BLOCK(BEG_ADDRESS, data_block);       // Read the very first block
                                                         // of data ton the SD card

    // WATCH OUT FOR BYTE OREDER
    // THE CODE IS COMPILED FOR LITTLE ENDIAN
    // THEREFORE 0X00000001 != 1
    // AND       0X01000000 == 1
    // BECAUSE ALL THE SIZES ARE IN BIG ENDIAN
    // A FUNCTION IS USED FOR THE CONVERSION

    LAST_LOC = ((uint32_t*) data_block)[0];
    LAST_LOC = ltobe(LAST_LOC);

    SD_READ_SINGLE_BLOCK(BEG_ADDRESS + 1, data_block);   // First file should be exactly
    FILE_SIZE = ((uint32_t*) data_block)[0];             // after LAST_LOC
    FILE_SIZE = ltobe(FILE_SIZE);

    FILE_BEG = 2 * sizeof(uint32_t);                     // Skip file size and CRC
    CUR_LOC = FILE_BEG;

    free(data_block);

}

uint32_t FS_next_file(){
    uint8_t* data_block = malloc(sizeof(uint8_t) * BLOCK_SIZE);

    if(FILE_BEG + FILE_SIZE > LAST_LOC){
        return 0x08;
    }else{
        FILE_BEG = FILE_BEG + FILE_SIZE;
    }
    CUR_LOC = FILE_BEG;

    SD_READ_SINGLE_BLOCK(CUR_LOC, data_block);
    FILE_SIZE = ((uint32_t*) data_block)[0];
    free(data_block);

    return 0x00;
}

uint32_t FS_prev_file(){
    uint8_t* data_block = malloc(sizeof(uint8_t) * BLOCK_SIZE);

    if(FILE_BEG - FILE_SIZE < ltobe(0x01)){
        return 0x08;
    }else{
        FILE_BEG = FILE_BEG - FILE_SIZE;
    }
    CUR_LOC = FILE_BEG;

    SD_READ_SINGLE_BLOCK(FILE_BEG, data_block);
    FILE_SIZE = ((uint32_t*) data_block)[0];
    free(data_block);
    return 0x00;
}

uint32_t FS_get_file_size(){
    return FILE_SIZE;
}

void FS_read_block(uint8_t* data_block){
    SD_READ_SINGLE_BLOCK(CUR_LOC, data_block);
    CUR_LOC += 1;
}

uint8_t FS_upload_block(uint8_t* data_block, uint32_t address){
    for(int i = 0 ; i < BLOCK_SIZE ; ++ i){
        data_block[i] = UART0_read_byte();
    }

    SD_WRITE_BLOCK(address, data_block);
    return 0x00;
}

uint32_t FS_get_last_location(){
    return LAST_LOC;
}
uint32_t FS_format(){
    SD_format();
    LAST_LOC = 0x01;
    return 0x00;
}

uint32_t FS_upload_file(){
    uint8_t* data_block = malloc(sizeof(uint8_t) * BLOCK_SIZE);
    UART0_write_bytes(&LAST_LOC, sizeof(LAST_LOC));

    FS_upload_block(data_block, LAST_LOC);
    LAST_LOC += 1;

    uint32_t f_blocks = ((uint32_t*) data_block)[0];
    f_blocks = ltobe(f_blocks);
    UART0_write_bytes(&f_blocks, sizeof(f_blocks));

    uint32_t f_crc = ((uint32_t*) data_block)[1];
    UART0_write_bytes(&f_crc, sizeof(f_crc));

    for(int i = 1 ; i < f_blocks ; ++ i){
        FS_upload_block(data_block, LAST_LOC);

        LAST_LOC += 1;
        ((uint32_t*)data_block)[0] = ltobe(LAST_LOC);
        SD_WRITE_BLOCK(BEG_ADDRESS, data_block);

        UART0_write_byte(0x00);
    }

    free(data_block);

    return 0x00;
}
