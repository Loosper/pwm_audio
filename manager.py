import serial

# Make a cool command line tool for
# interfacing with the SD card.
# Make it possible to send commands
# from the SD card and recieve the response
# Add the ability to upload music files
#
# >> CMD0
# 0x00
# >> CMD8 (operating voltage)
# 0x00 0x00 0x80 0x00
# >> upload ./cancer.wav
# uploading...
# Done!
# >> exit
# bye!

cmd_dict = {
    'CMD0': 0,
    'CMD1': 1,
    'CMD8': 8,
    'CMD58': 58,
    'ACMD41': 41
    }


def UART_read_byte(conn):
    return conn.read()


def UART_write_byte(conn, byte):
    conn.write(bytes([byte]))


def main():
    BAUD = 9600
    conn = serial.Serial('/dev/ttyACM0', BAUD)

    try:
        while True:
            print(UART_read_byte(conn))
    except KeyboardInterrupt:
        conn.close()
        print('bye!')

#     try:
#         while True:
#             cmd_str = input('>> ')
#             cmd_num = 0
#
#             try:
#                 cmd_num = cmd_dict[cmd_str]
#                 UART_write_byte(conn, cmd_num)
#                 print(UART_read_byte(conn))
#             except KeyError:
#                 print('No such command!')
#                 print('Supported commands:')
#                 print(' '.join(cmd_dict.keys()))
#
#     except KeyboardInterrupt:
#         conn.close()
#         print('\nbye!')


if __name__ == '__main__':
    main()
