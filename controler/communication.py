import serial

class aqDronePort(serial.Serial):
	"""docstring for aqDronePort"""
	def __init__(self, port, baudrate=57600, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=None):
		super(aqDronePort, self).__init__(port=port,baudrate=baudrate,parity=parity,stopbits=stopbits,bytesize=bytesize,timeout=timeout)

	def msgSend(self,m):
		self.write("<M%s>"%str(m))


dronePort = aqDronePort(
	# port='/dev/ttyUSB0',\
	port='/dev/rfcomm0',
	baudrate=57600,\
	parity=serial.PARITY_NONE,\
	stopbits=serial.STOPBITS_ONE,\
	bytesize=serial.EIGHTBITS,\
	timeout=None
)
