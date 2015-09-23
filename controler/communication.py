import serial

dronePort = serial.Serial(
	port='/dev/ttyUSB0',\
	baudrate=57600,\
	parity=serial.PARITY_NONE,\
	stopbits=serial.STOPBITS_ONE,\
	bytesize=serial.EIGHTBITS,\
	timeout=None
)