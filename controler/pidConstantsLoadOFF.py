from eepromTools import *

print 'Setting pid constants loading: OFF'
if not(writeOneByte(dronePort,0,0x00)) or \
	not(writeOneByte(dronePort,1,0x00)) or \
	not(writeOneByte(dronePort,2,0x00)) or \
	not(writeOneByte(dronePort,3,0x00)):
	print 'ERROR!'
else:
	print 'DONE! :-)'