// SD Card commands
// http://www.dejazzer.com/ee379/lecture_notes/lec12_sd_card.pdf
// http://elm-chan.org/docs/mmc/mmc_e.html
// https://openlabpro.com/guide/interfacing-microcontrollers-with-sd-card/
#include <stdio.h>

#define SDBEG                     0x40  // Hex value of the leading 01xxxxxx needed in every command
#define GO_IDLE_STATE             0x00  // Software reset
#define SEND_OP_COND              0x01  // Initiate initialization process
#define APP_SEND_OP_COND          0x29  // Only for SDC. Initiate initialization process.
#define SEND_IF_COND              0x08  // For only SDC V2. Check voltage range
#define SEND_CSD                  0x09  // Read CSD register
#define SEND_CID                  0x0A  // Read CID register
#define STOP_TRANSMISSION         0x0C  // Stop to read data
#define SEND_STATUS               0x0D  // Read CID register
#define SET_BLOCKLEN              0x10  // Change R/W block size
#define READ_SINGLE_BLOCK         0x11  // Read a block
#define READ_MULTIPLE_BLOCK       0x12  // Read multiple blocks
#define SET_WR_BLOCK_ERASE_COUNT  0x17  // For only SDC. Define number of blocks to pre-erase
                                        // with next multi-block write command.
#define WRITE_BLOCK               0x18  // Write a block
#define WRITE_MULTIPLE_BLOCK      0x19  // Write multiple blocks
#define APP_CMD                   0x37  // Leading command of ACMD<n> command
#define READ_OCR                  0x3A  // Read OCR
#define SWITCH_CRC                0x3B  // Switch CRC
//ACMD<n> means a command sequense of CMD55-CMD<n>.

#define MSG_LENGTH 6    // Every command has a constant length of 6 bytes
#define BLOCK_SIZE 512  //Read and write block size [bytes]

void SD_init();
void SD_SEND_DUMMY();
uint32_t SD_GO_IDLE_STATE();
uint32_t SD_SEND_IF_COND();
uint32_t SD_APP_SEND_OP_COND();
uint32_t SD_SET_BLOCKLEN();
uint32_t SD_READ_SINGLE_BLOCK(uint32_t, uint8_t*);
uint32_t SD_WRITE_BLOCK(uint32_t, uint8_t*);
uint32_t SD_format();
