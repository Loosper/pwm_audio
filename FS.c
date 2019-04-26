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

#include "FS.h"
#include "SD.h"

uint32_t FILE_BEG;            // Beggining of the current file
uint32_t FILE_SIZE;           // Size of the current file
uint32_t CUR_LOC;             // Location in the current file (in BLOCK_SIZE)

void FS_next_file(){
    uint8_t* data_block;

    FILE_BEG = FILE_BEG + FILE_SIZE;
    CUR_LOC = FILE_BEG;

    data_block = SD_READ_SINGLE_BLOCK(CUR_LOC);
    FILE_SIZE = (uint32_t) data_block;
}

void FS_prev_file(){
    uint8_t* data_block;

    FILE_BEG = FILE_BEG - FILE_SIZE;
    if(FILE_BEG < 0) FILE_BEG = 0;
    CUR_LOC = FILE_BEG;

    data_block = SD_READ_SINGLE_BLOCK(FILE_BEG);
    FILE_SIZE = (uint32_t) data_block;
}

void FS_read_block(uint8_t* data_block){
    data_block = SD_READ_SINGLE_BLOCK(CUR_LOC);
}

uint8_t FS_upload_file();
// File should be send as the file system specifies.
// After the file is saved to the SD card, a CRC will
// calculated and if it matches the one send from the
// PC LAST_POS will be increased accordingly and a
// 0x00 byte will be sent to the PC indicating success
// If not LAS_POS will remain unchanged and a 0xFF byte
// will be sent to the PC indicating failiure

