import serial
import csv
from eepromTools import *

pidValues = []
for line in csv.reader(open('pidConstantsValues.csv','r')):
	pidValues.append([ float(el) for el in line])

print pidValues
print 'Sending pid values...',
if writePidValues(dronePort,pidValues):
	print 'OK :-)'
else:
	print 'ERROR'