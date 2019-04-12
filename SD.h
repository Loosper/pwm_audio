#define SDBEG   0x40 // Hex value of the leading 01xxxxxx needed in every command
#define CMD0    0x00 // Software reset
#define CMD1    0x01 // Initiate initialization process
#define ACMD41  0x29 // Only for SDC. Initiate initialization process.
#define CMD8    0x08 // For only SDC V2. Check voltage range
#define CMD9    0x09 // Read CSD register
#define CMD10   0x0A // Read CID register
#define CMD12   0x0C // Stop to read data
#define CMD16   0x10 // Change R/W block size
#define CMD17   0x11 // Read a block 
#define CMD18   0x12 // Read multiple blocks
#define ACMD23  0x17 // For only SDC. Define number of blocks to pre-erase 
                     // with next multi-block write command.
#define CMD24   0x18 // Write a block 
#define CMD25   0x19 // Write multiple blocks
#define CMD55   0x37 // Leading command of ACMD<n> command
#define CMD58   0x3A // Read OCR
//ACMD<n> means a command sequense of CMD55-CMD<n>.

#define MSG_LENGTH 6 // Every command has a constant length of 6 bytes
