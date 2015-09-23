from eepromTools import *

print 'PidValues:', readPidValues(dronePort)
print 'MAGIC:', ord(readOneByte(dronePort,0)), ord(readOneByte(dronePort,1)), ord(readOneByte(dronePort,2)), ord(readOneByte(dronePort,3))