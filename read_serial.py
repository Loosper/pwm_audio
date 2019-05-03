import serial
import time
import wave
import sys
import subprocess

from settings import *

con = serial.Serial(
    TTY, BAUD,
    # timeout=0.1,
)

try:
    con.read(16)
    time.sleep(0.2)
    con.write(b'\x00')
    con.write(b'\x08')
    con.write(b'\x29')
    con.read(12)
    # con.write(b'\xAB')
    # time.sleep(1)

    while True:
        con.write(b'\xAA')
        print(f'0x{int.from_bytes(con.read(4), byteorder="little"):02x}')
        time.sleep(0.4)
except KeyboardInterrupt:
    con.close()
    print('bye')
