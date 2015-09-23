import re
import struct
from communication import *

def writeOneByte(ser,writeAddress,valueToWrite):
	ser.write("<EI%d:%d>"%(writeAddress,valueToWrite))
	ser.write("<EO%d>"%writeAddress)
	while True:
		data = re.findall(r"\<E([-+]?\d*\.\d+|\d+),([-+]?\d*\.\d+|\d+)\>", ser.readline())
		if len(data) > 0:
			if int(float(data[-1][0])) == writeAddress:
				return int(float(data[-1][1])) == valueToWrite

def writeBytes(serialPort,startAdress,data):
	offsetPos = 0
	for singleByte in data:
		# print startAdress+offsetPos,ord(singleByte)
		if not(writeOneByte(serialPort,startAdress+offsetPos,ord(singleByte))):
			return False
		offsetPos += 1
	return True

def writeFloat(serialPort,floatWriteAdres,floatValue):
	data = struct.pack('<f',floatValue)
	real_floatWriteAdres = floatWriteAdres * len(data)
	return writeBytes(serialPort,real_floatWriteAdres,data)

def writePidValues(serialPort,valuesArray):
	elemNo = 1
	for pidControler in valuesArray:
		for constant in pidControler:
			if not(writeFloat(serialPort,elemNo,constant)):
				print 'Error while writing cno(%d)= %f'%(elemNo,constant)
				return False
			elemNo += 1
	return True

def cleanEEPROM(serialPort):
	for adrForClean in range(128):
		writeOneByte(serialPort,adrForClean,7)

def dumpEEPROM(serialPort):
	for adrForDump in range(128):
		serialPort.write("<EO%d>"%adrForDump)
		while True:
			data = re.findall(r"\<E([-+]?\d*\.\d+|\d+),([-+]?\d*\.\d+|\d+)\>", serialPort.readline())
			if len(data) > 0:
				if int(float(data[-1][0])) == adrForDump:
					val = int(float(data[-1][1])) 
					print '%d:%d'%(adrForDump,val)
					break

def readOneByte(ser,readAdr):
	ser.write("<EO%d>"%readAdr)
	while True:
		data = re.findall(r"\<E([-+]?\d*\.\d+|\d+),([-+]?\d*\.\d+|\d+)\>", ser.readline())
		if len(data) > 0:
			if int(float(data[-1][0])) == readAdr:
				return chr(int(float(data[-1][1])))

def readFloat(serialPort,floarReadAdr):
	real_floarReadAdr = floarReadAdr * 4
	data = readOneByte(serialPort,real_floarReadAdr)
	data += readOneByte(serialPort,real_floarReadAdr+1)
	data += readOneByte(serialPort,real_floarReadAdr+2)
	data += readOneByte(serialPort,real_floarReadAdr+3)
	return struct.unpack('<f',data)[0]


def readPidValues(serialPort):
	out = []
	elemNo = 1
	for i in range(4):
		aout = []
		for j in range(3):
			aout.append(readFloat(serialPort,elemNo))
			elemNo += 1
		out.append(aout)
	return out