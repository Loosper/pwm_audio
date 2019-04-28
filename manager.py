import sys
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
#  HEADER_BLOCK        0X00
#   DATA_BLOCK         0X00
#       .
#       .
#       .
#   DATA_BLOCK         0X00
#   (final one)
#                      CRC32
#
# If the calculated CRC from the Arduino and
# the one from the uploader match the file is
# saved and LAST_LOC is progressed with the
# size of the file. Else LAST_LOC is left unchanged


BLOCK_SIZE = 512    # bits
HEADER_SIZE = 8      # bits

cmd_dict = {
    'GO_IDLE_STATE':       bytes([0x00]),
    'SEND_IF_COND':        bytes([0x08]),
    'APP_SEND_OP_COND':    bytes([0x29]),
    'READ_SINGLE_BLOCK':   bytes([0x11]),
    'WRITE_BLOCK':         bytes([0x18])
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
        # print(UART_read_byte(conn))


def upload_file(conn, file_name):
    print(file_name)
    desc = wave.open(file_name, 'rb')  # Open file descriptor
    size = desc.getnframes()           # Get file size
    data = desc.readframes(size)       # Read file data
    crc = CRC32().calculate(data)      # Calculate 32 bit CRC

    size_bytes = struct.pack('>I', size // BLOCK_SIZE) # Convert to bytes object
    crc_bytes = struct.pack('>I', crc)                 # for transmission over UART
    print('CRC32:', '0x' + crc_bytes.hex())            # Print CRC32 in hex

    last_loc = UART_read_bytes(conn, 4)                # Read the location of the last
    print('LAST_LOC:', '0x' + last_loc.hex())          # file on the FS and print it


    head_block = size_bytes + crc_bytes + data[:BLOCK_SIZE - HEADER_SIZE]
    UART_write_bytes(conn, head_block, BLOCK_SIZE)

    crc_arduino = UART_read_bytes(conn, 4)             # Get CRC32 from Arduino
    print('CRC32_ARDUINO:', '0x' + crc_arduino.hex())
    print('Uploading.', end='', flush=True)

    for i in range(BLOCK_SIZE - HEADER_SIZE, size, BLOCK_SIZE):
        data_block = data[i:i + BLOCK_SIZE]
        UART_write_bytes(conn, data_block, BLOCK_SIZE)
        if UART_read_byte(conn) == bytes([0]):
            print('.', end='', flush=True)
        else:
            print('x', end='', flush=True)


def main():
    BAUD = 9600
    conn = serial.Serial('/dev/ttyACM0', BAUD)

    # upload_file(conn, 'koji_8bit_mono.wav')

    try:
        for i in range(4):
            resp_byte = UART_read_byte(conn)
            resp = int.from_bytes(resp_byte, 'little')
            print('0x{:02x}: {}'.format(resp, resp_dict[resp]))

        while True:
            cmd_str = input('>> ').split(' ')
            cmd = 0

            try:
                cmd = cmd_dict[cmd_str[0]]
                UART_write_byte(conn, cmd)
                if cmd == cmd_dict['WRITE_BLOCK']:
                    upload_file(conn, cmd_str[1])

                resp = int.from_bytes((UART_read_byte(conn)), 'little')

                print('0x{:02x}: {}'.format(resp, resp_dict[resp]))
            except KeyError:
                print('No such command\n')
                print('Supported commands:')
                print(' '.join(cmd_dict.keys()))

    except KeyboardInterrupt:
        conn.close()
        print('\nbye!')


if __name__ == '__main__':
    main()
