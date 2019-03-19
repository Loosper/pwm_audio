import serial
import time


BAUD = 57600
con = serial.Serial(
    '/dev/ttyUSB0', BAUD, timeout=1,
)

try:
    i = 0
    while True:
        i = (i + 1) % 255
        data = i.to_bytes(1, byteorder='big')

        con.write(data)
        print(data)
        time.sleep(0.01)
except KeyboardInterrupt:
    con.close()
    print('bye')
