#include <stdint.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

#include "SD.h"
#include "UART.h"
#include "SPI.h"
// TODO
// 1) In SD_read_resp, find a way to return
//    the rest of the responce message without
//    the extra dummy bytes(0xFF)
//
// 2) If a commad isn't accepted 20 times
//    return an error responce


// Dummy bytes needed to prepare SD card
uint8_t dummy_mosi_data[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

uint8_t msg[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


void SD_write_cmd(uint8_t *mosi_data){
    for(int i = 0 ; i < MSG_LENGTH ; ++ i){
        SPI_write_byte(mosi_data[i]);
    }
}

int SD_read_resp(){
    int rcv_flag = 0;
    uint8_t head;         //First byte of response which is a status
                          // message indicating if the command was
                          // recieved successfully
    uint32_t data;        // Rest bytes of response.

    for(int i = 0 ; i < 6 ; rcv_flag ? ++ i : i){
        uint8_t recv_byte = SPI_read_byte();

        if (recv_byte != 0xFF && !rcv_flag){
            rcv_flag = 1;

            head = recv_byte;
        }else if(rcv_flag && i < 5){
            data = data << 8;
            data = recv_byte;
        }
    }

    return head;
}

uint8_t* SD_build_msg(uint8_t head, uint32_t data, uint8_t *msg){
    msg[0] = head | SDBEG;

    msg[1] = data >> 24;
    msg[2] = data >> 16;
    msg[3] = data >> 8;
    msg[4] = data;

    // When in SPI mode only CRC for CMD0 and CMD8 is needed
    // Therefore we have it precalculated instead of wasting
    // time calculating it on the spot.
    if (head == GO_IDLE_STATE){
        msg[5] = 0x95;
    }else if (head == SEND_IF_COND){
        msg[5] = 0x87;
    }else if(head == SET_BLOCKLEN){
        msg[5] = 0xFF;
    }else{
        msg[5] = 0x00;
    }

    return msg;
}

void SD_SEND_DUMMY(){
    SD_write_cmd(dummy_mosi_data);     // Send a total of 12 dummy bytes
    SD_write_cmd(dummy_mosi_data);
}

uint8_t SD_GO_IDLE_STATE(){
    SD_build_msg(GO_IDLE_STATE, 0x00000000, msg);
    SD_write_cmd(msg);
    return SD_read_resp();
}

uint8_t SD_SEND_IF_COND(){
    SD_build_msg(SEND_IF_COND, 0x000001AA, msg);
    SD_write_cmd(msg);
    return SD_read_resp();
}

uint8_t SD_APP_SEND_OP_COND(){
    uint8_t resp = 0xFF;
    do{
        SD_build_msg(APP_CMD, 0x00000000, msg);
        SD_write_cmd(msg);
        SD_read_resp();
        SD_build_msg(APP_SEND_OP_COND, 0x40000000, msg);
        SD_write_cmd(msg);
        resp = SD_read_resp();
    }while(resp != 0x00);

    return resp;
}

uint8_t SD_SET_BLOCKLEN(){
    SD_build_msg(SET_BLOCKLEN, 0x00000000 | BLOCK_SIZE, msg);
    SD_write_cmd(msg);
    return SD_read_resp();
}

uint8_t SD_READ_SINGLE_BLOCK(uint32_t address, uint8_t* data){
    uint8_t resp = 0x00;

    // The logic here is a bit different from the intitialize
    // routines. The READ_SINGLE_BLOCK command first sends a
    // 0x00 to indicate that the command is correct and after
    // x bytes send a 0xFE message indicating that the next
    // BLOCK_SIZE bytes are from the SD card's memory.
    // Thus instead of reading 6 byte responses as in previous
    // commands we need to read byte by byte because of the
    // unknown number of separator bytes between 0x00 and 0xFE
    // responses.
    SPI_write_byte(0xFF);

    do{
        SD_build_msg(READ_SINGLE_BLOCK, address, msg);
        SD_write_cmd(msg);

        do{
            resp = SPI_read_byte();
        }while(resp == 0xFF);
    }while(resp != 0x00);

    do{
        resp = SPI_read_byte();
    }while(resp != 0xFE);

    for(int i = 0 ; i < BLOCK_SIZE ; ++ i){
        data[i] = SPI_read_byte();
    }

    SPI_write_byte(0xFF);
    return resp;
}

uint8_t SD_WRITE_BLOCK(uint32_t address, uint8_t* data){
    uint8_t resp = 0x00;
    SPI_write_byte(0xFF);

    do{
        SD_build_msg(WRITE_BLOCK, address, msg);
        SD_write_cmd(msg);
        do{
            resp = SPI_read_byte();
        }while(resp == 0xFF);
    }while(resp != 0x00);

    SPI_write_byte(0xFE); // First byte indicating begining of data block

    for(int i = 0 ; i < BLOCK_SIZE ; ++ i){
        SPI_write_byte(data[i]);
    }

    // After the data is sent to the SD card it will first
    // take some time to process it during which the line
    // will be held HIGH(1). After that the line will be held
    // LOW(0) indicating that the data is being written on
    // to the SD card. After the MISO line is brought back
    // HIGH(1) we can begin to send new commands to the card
    //
    //
    //  0xFF  .  0xFF . 0xE5  . 0x00  . 0x07  . 0xFF
    //        .       .       .       .       .
    //        .       .       .       .       .
    // -------.-------.---+   .       .    +--.-----------
    //        .       .   |   .       .    |  .
    //        .       .   |   .       .    |  .
    //        .       .   |   .       .    |  .
    //        .       .   |   .       .    |  .
    //        .       .   |   .       .    |  .
    //        .       .   |___._______.____|  .
    //
    //    *PROCCESSING*        *WRITING*        *READY*
    //
    // The ASCII art above shows how between *PROCESSING* and
    // *WRITTING* and between *WRITTING* and *READY* we can get
    // data different from 0xFF and 0x00 respectively due to
    // segmentation between each byte read.


    do{
        resp = SPI_read_byte();
    }while(resp == 0xFF);

    do{
        resp = SPI_read_byte();
    }while(resp == 0x00);

    return resp;
}

void SD_init(){

    SD_SEND_DUMMY();
    UART0_write_byte(SD_GO_IDLE_STATE());
    UART0_write_byte(SD_SEND_IF_COND());
    UART0_write_byte(SD_APP_SEND_OP_COND());
    UART0_write_byte(SD_SET_BLOCKLEN());

    uint8_t* block_data = malloc(sizeof(uint8_t) * BLOCK_SIZE);
    for(int i = 0 ; i < BLOCK_SIZE ; ++ i)
            block_data[i] = 0xAA;

    SD_WRITE_BLOCK(0x0000FF00, block_data);

    SD_READ_SINGLE_BLOCK(0x0000FF00, block_data);
    UART0_write_byte(block_data[0]);
    UART0_write_byte(block_data[1]);
    UART0_write_byte(block_data[2]);
}
