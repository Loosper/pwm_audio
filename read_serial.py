import serial
import time
import wave
import sys
import subprocess

# print(file.getparams())

BAUD = 9600
con = serial.Serial(
    '/dev/ttyUSB2', BAUD,
    timeout=0.01,
)
file = wave.open('file.wav', 'rb')

# print(file.readframes(1))
# sys.exit()

# time.sleep(1)
print('starting')
# subprocess.Popen(['aplay', 'file.wav'])

def play(x):
    con.write(x.to_bytes(1, byteorder='big'))
    return con.read(1)

try:
    while True:
        sample = file.readframes(1)
        if not sample:
            break
        ret = play(sample[0])
        # print(int.from_bytes(ret, byteorder='big'), sample[0])
except KeyboardInterrupt:
    con.close()
    print('bye')
