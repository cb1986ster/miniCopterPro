from eepromTools import *

print 'Setting pid constants loading: ON'
if not(writeOneByte(dronePort,0,0xAA)) or \
	not(writeOneByte(dronePort,1,0xAA)) or \
	not(writeOneByte(dronePort,2,0xAA)) or \
	not(writeOneByte(dronePort,3,0xAA)):
	print 'ERROR!'
else:
	print 'DONE! :-)'