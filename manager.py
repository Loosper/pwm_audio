import sys
import math
import wave
import serial
import struct
from binascii import hexlify

from settings import *

BLOCK_SIZE = 512    # bytes
RESP_SIZE = 4


# this is a dumb scipt, no need for main checks and so on
try:
    conn = serial.Serial(TTY, BAUD)
except serial.SerialException:
    print(f'Error connecting to {TTY}')
    sys.exit()


responses = {
    0x00: 'Ready',
    0x01: 'Idle State',
    0x02: 'Erase Reset',
    0x04: 'Illeagal Command',
    0x08: 'Command CRC Error',
    0x10: 'Erase Sequence Error',
    0x20: 'Address Error',
    0x21: 'Pong',
    0x40: 'Parameter Error'
}

def upload_file(file_name):
    with wave.open(file_name, 'rb') as file:
        size = file.getnframes()

        status = conn.read(1)
        if status == b'\x01':
            print('There\'s room for more files.')
        else:
            print(status)
            print('file limit reached')
            return

        bytes_available = int.from_bytes(conn.read(4), byteorder='little')
        print(f'Free bytes on SD card: {bytes_available}')

        conn.write(size.to_bytes(4, byteorder='little'))
        status = conn.read(1)
        if status == b'\x01':
            print(f'Bytes to send: {size}')
        else:
            print(status)
            print('Not enough space on SD card')
            return

        # blocks = math.ceil(size / BLOCK_SIZE)
        # TODO: most likely an off by 1 error on the controller.
        # here we trust it is correct just to complete the upload
        cur = 0
        while True:
            status = conn.read(1)

            if status != b'\x01':
                break

            print(f'Sending block {cur}')
            data = file.readframes(BLOCK_SIZE)

            conn.write(data)
            cur += 1

        assert status == b'\x02'
        print('Transfer complete')

def read_response():
    resp = conn.read(RESP_SIZE)
    resp_code = int.from_bytes(resp, 'little')
    return resp_code

def upload_cmd(params):
    try:
        upload_file(params[0])
    except IndexError:
        print('Please enter filename')
        return
    except FileNotFoundError:
        print(f'No such file {params[0]}')
        return

    resp_code = read_response()
    print(format_response(resp_code))

def format_response(resp_code):
    try:
        return '0x{:02x}: {}'.format(resp_code, responses[resp_code])
    except KeyError:
        return f'{hex(resp_code)}'

def formatted_cmd(params):
    resp_code = read_response()
    print(format_response(resp_code))

def info_cmd(params):
    resp = conn.read(RESP_SIZE)
    print(f'0x{resp.hex()}')


commands = {
    'GO_IDLE_STATE':    [formatted_cmd, b'\x00', 'Software reset'],
    'SEND_IF_COND':     [formatted_cmd, b'\x08', 'Check voltage range'],
    'APP_SEND_OP_COND': [formatted_cmd, b'\x29', 'Start initialization process'],
    'FORMAT':           [formatted_cmd, b'\xAA', 'Format SD card'],
    'INFO':             [info_cmd,      b'\xAB', 'Get info about SD card'],
    'FILE_SIZE':        [info_cmd,      b'\xAC', 'Get size of last file'],
    'PING':             [formatted_cmd, PING, 'Check for connectivity'],
    'UPLOAD':           [upload_cmd,    b'\x18', 'Upload a file'],
}

def main():
    print('Waiting for SD initialisation')
    # connecting to an arduino over UART will reset it
    # this is it's startup message
    for i in range(4):
        resp_code = read_response()
        print(format_response(resp_code))

    # set up SD card. Needed for some reason
    conn.write(commands['GO_IDLE_STATE'][1])
    conn.write(commands['SEND_IF_COND'][1])
    conn.write(commands['APP_SEND_OP_COND'][1])
    conn.read(12)

    print('Enter a command:')
    while True:
        cmd_str = input('>> ').split(' ')

        try:
            cmd = commands[cmd_str[0]]
            conn.write(cmd[1])
            cmd[0](cmd_str[1:])
        except KeyError:
            print('No such command\n')
            print('Commands:')
            for key in commands.keys():
                print('{:20}{}'.format(key, commands[key][1]))


if __name__ == '__main__':
    try:
        main()
    except (KeyboardInterrupt, EOFError):
        conn.close()
        print('\nbye!')
