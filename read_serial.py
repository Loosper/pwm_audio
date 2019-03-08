import serial


BAUD = 57600
# port changes when unplugged
try:
    con = serial.Serial('/dev/ttyUSB0', BAUD)
except:
    con = serial.Serial('/dev/ttyUSB1', BAUD)

while True:
	print(con.read(1))

