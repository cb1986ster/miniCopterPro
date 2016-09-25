import serial

class aqDronePort(serial.Serial):
	"""docstring for aqDronePort"""
	def __init__(self, port, baudrate=57600, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=None):
		super(aqDronePort, self).__init__(port=port,baudrate=baudrate,parity=parity,stopbits=stopbits,bytesize=bytesize,timeout=timeout)

	def msgSend(self,m):
		buffer = "<M%s>"%str(m)
		self.write(buffer)
		return buffer

	def paramsSend(self,paramType,paramData):
		paramData = [str(e) for e in paramData]
		buffer = "<%s%s>"%(str(paramType),str(':'.join(paramData)))
		self.write(buffer)
		return buffer


dronePort = aqDronePort(
	# port='/dev/ttyUSB0',\
	port='/dev/rfcomm0',\
	baudrate=57600,\
	parity=serial.PARITY_NONE,\
	stopbits=serial.STOPBITS_ONE,\
	bytesize=serial.EIGHTBITS,\
	timeout=None
)
