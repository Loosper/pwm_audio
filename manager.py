import sys
import math
import wave
import serial
import struct
from binascii import hexlify
from PyCRC.CRC32 import CRC32
#       UPLOAD STRUCTURE
#
#     MESSAGE        RESPONCE
# ------------------------------
#  WRITE_BLOCK       LAST_LOC
#  HEADER_BLOCK        SIZE
#                      CRC32
#
#   DATA_BLOCK         0X00
#       .
#       .
#       .
#   DATA_BLOCK         0X00
#   (final one)
#
#
# If the calculated CRC from the Arduino and
# the one from the uploader match the file is
# saved and LAST_LOC is progressed with the
# size of the file. Else LAST_LOC is left unchanged


BLOCK_SIZE = 512    # bytes
HEADER_SIZE = 8     # bytes
RESP_SIZE = 4

cmd_dict = {
    'GO_IDLE_STATE':       bytes([0x00]),
    'SEND_IF_COND':        bytes([0x08]),
    'APP_SEND_OP_COND':    bytes([0x29]),
    'UPLOAD':              bytes([0x18]),
    'FORMAT':              bytes([0xAA]),
    'LAST_LOC':            bytes([0xAB]),
    'FILE_SIZE':           bytes([0xAC]),
    }

desc_dict = {
    'GO_IDLE_STATE':       'Software reset',
    'SEND_IF_COND':        'Check voltage range',
    'APP_SEND_OP_COND':    'Init initialization process',
    'UPLOAD':              'Upload a file',
    'FORMAT':              'Format SD card',
    'LAST_LOC':            'Get location of last file'
    }

resp_dict = {
    0x00: 'Ready',
    0x01: 'Idle State',
    0x02: 'Erase Reset',
    0x04: 'Illeagal Command',
    0x08: 'Command CRC Error',
    0x10: 'Erase Sequence Error',
    0x20: 'Address Error',
    0x40: 'Parameter Error'
        }


def UART_read_byte(conn):
    return conn.read()

def UART_read_bytes(conn, size):
    data = bytes()
    for i in range(size):
        data += UART_read_byte(conn)

    return data

def UART_write_byte(conn, byte):
    conn.write(byte)


def UART_write_bytes(conn, data, size):
    for byte in data:
        UART_write_byte(conn, bytes([byte]))
        # print(UART_read_byte(conn).hex())

def get_file_params(file_name):
    desc = wave.open(file_name, 'rb')                   # Open file descriptor
    size = desc.getnframes()                            # Get file size
    data = desc.readframes(size)                        # Read file data
    crc = CRC32().calculate(data)                       # Calculate 32 bit CRC
    desc.close()
    return size, data, crc


def upload_file(conn, file_name):
    size, data, crc = get_file_params(file_name)             # Get file params

    blocks = math.ceil((size + HEADER_SIZE) / BLOCK_SIZE)    # Convert size in
                                                             # block units
    print(blocks)
    blocks_bytes = struct.pack('>I', blocks)            # Convert to bytes object
    print('BLOCKS:', '0x' + blocks_bytes.hex())         # for transmission over UART

    crc_bytes = struct.pack('>I', crc)
    print('CRC32:', '0x' + crc_bytes.hex())

    # When the UPLOAD command in sent to the
    # AVR it will respond with 4 bytes indicating
    # the position of the last file in the FS
    last_loc = UART_read_bytes(conn, 4)                 # Read the location of the last
    print('LAST_LOC:', '0x' + last_loc.hex())           # file on the FS and print it

    # Send header over UART
    head_block = blocks_bytes + crc_bytes + data[:BLOCK_SIZE - HEADER_SIZE]
    UART_write_bytes(conn, head_block, BLOCK_SIZE)

    # The bellow two prints are just to make sure that
    # the header has been transmited succesfully

    blocks_arduino = UART_read_bytes(conn, 4)             # Get blocks count from Arduino
    print('BLOCKS_ARDUINO:', '0x' + blocks_arduino.hex())

    crc_arduino = UART_read_bytes(conn, 4)                # Get CRC32 from Arduino
    print('CRC32_ARDUINO:', '0x' + crc_arduino.hex())

    # Begin upload proccess
    print('Uploading | ', end='', flush=True)
    counter = 1
    for i in range(1, blocks):
        # Because in the first block we include the header
        # this affects all of the rest making them 'uneven'.
        # Because of this we subtract it from the begging
        # and ending index of each of the nex blocks
        beg_ind = i * BLOCK_SIZE - HEADER_SIZE
        end_ind = (i + 1) * BLOCK_SIZE - HEADER_SIZE
        data_block = data[beg_ind:end_ind]

        if len(data_block) < BLOCK_SIZE:
           data_block += bytes(BLOCK_SIZE - len(data_block))

        UART_write_bytes(conn, data_block, BLOCK_SIZE)
        # When the block is transmitted the AVR will
        # respond with a 0x00 indicating success
        if UART_read_byte(conn) == bytes([0]):
            counter += 1
            if counter % (blocks // 50) == 0:
                counter = 0
                print('#', end='', flush=True)
        else:
            print('x', end='', flush=True)

    print()

def format_response(resp_code):
    return '0x{:02x}: {}'.format(resp_code, resp_dict[resp_code])

def read_response(conn):
    resp = UART_read_bytes(conn, RESP_SIZE)
    resp_code = int.from_bytes(resp[0:1], 'little')
    return resp_code

def main():
    BAUD = 76800
    conn = serial.Serial('/dev/ttyACM0', BAUD)

    try:
        for i in range(4):
            resp_code = read_response(conn)
            print(format_response(resp_code))

        while True:
            cmd_str = input('>> ').split(' ')

            try:
                cmd = cmd_dict[cmd_str[0]]
                UART_write_byte(conn, cmd)

                # UPLOAD is the only command that recieves
                # a second parameter
                if cmd == cmd_dict['UPLOAD']:
                    upload_file(conn, cmd_str[1])
                    resp_code = read_response(conn)
                    print(format_response(resp_code))
                elif cmd == cmd_dict['LAST_LOC']:
                    print('0x' + UART_read_bytes(conn, RESP_SIZE).hex())
                elif cmd == cmd_dict['FILE_SIZE']:
                    print('0x' + UART_read_bytes(conn, RESP_SIZE).hex())
                else:
                    resp_code = read_response(conn)
                    print(format_response(resp_code))

            except KeyError:
                print('No such command\n')
                print('Commands:')
                for key in desc_dict.keys():
                    print('{:<20}{}'.format(key, desc_dict[key]))

    except KeyboardInterrupt:
        conn.close()
        print('\nbye!')


if __name__ == '__main__':
    main()
