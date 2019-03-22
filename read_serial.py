import serial
import time
import wave
import sys
import subprocess

# print(file.getparams())

BAUD = 57600
con = serial.Serial(
    '/dev/ttyUSB0', BAUD,
    timeout=0.01,
)
file = wave.open('file.wav', 'rb')

# print(file.readframes(1))
# sys.exit()

# time.sleep(1)
print('starting')
# subprocess.Popen(['aplay', 'file.wav'])
try:
    while True:
        sample = file.readframes(1)
        if not sample:
            break
        # time.sleep(0.000069)
        con.write(sample[0].to_bytes(1, byteorder='big'))
        # print(con.out_waiting)
        # print(sample)
        print(int.from_bytes(con.read(1), byteorder='big'), sample[0])
except KeyboardInterrupt:
    con.close()
    print('bye')
