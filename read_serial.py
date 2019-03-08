import serial


BAUD = 57600
con = serial.Serial(
    '/dev/ttyUSB0', BAUD, timeout=1,
)

try:
    i = 0
    while True:
        i = (i + 1) % 255
        con.write(bytes(i))
        con.flush()
        print(i)
        print(con.read(1))
except KeyboardInterrupt:
    con.close()
    print('bye')
