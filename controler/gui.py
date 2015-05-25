import pygame, sys,os, math
import thread
import time
import serial
import re

ioData = {
	'output' : {
		'roll':0.0,
		'pitch':0.0,
		'alt':0.0,
		'rotate':0.0,
		'gimbalRoll':0.0,
		'gimbalPitch':0.0,
		'msgs':[]
	},
	'input' : {
		'roll':0.0,
		'pitch':0.0,
		'lps':0.0,
		'gimbalRoll':0.0,
		'gimbalPitch':0.0
	}
}

pygame.init()

window = pygame.display.set_mode((800, 600)) # pygame.FULLSCREEN
pygame.display.set_caption('QuadCopter controll')
background = pygame.image.load('background.png').convert()

screen = pygame.display.get_surface()

clock= pygame.time.Clock()

isAppRunning = True

def communicationInput(ser):
	global isAppRunning
	global ioData
	while isAppRunning:
		line = ser.readline()
		data = re.findall(r"\<([-+]?\d*\.\d+|\d+),([-+]?\d*\.\d+|\d+),([-+]?\d*\.\d+|\d+),([-+]?\d*\.\d+|\d+),([-+]?\d*\.\d+|\d+)\>", line)
		if len(data) > 0:
			ioData['input']['pitch'] = float(data[-1][0])
			ioData['input']['roll'] = float(data[-1][1])
			ioData['input']['lps'] = ioData['input']['lps']*0.9 + float(data[-1][2])*0.1
		# print line

def communicationOutput(ser):
	global isAppRunning
	global ioData
	i = 0
	while isAppRunning:
		time.sleep(.001)
		while len(ioData['output']['msgs']) > 0:
			m = ioData['output']['msgs'].pop()
			ser.write("<%s>"%m)

		i+=1
		if i > 50:
			i=0
			m = "<p%0.3f:%0.3f:%0.3f:%0.3f:%0.3f:%0.3f>"%(
				ioData['output']['roll'],
				ioData['output']['pitch'],
				ioData['output']['rotate'],
				ioData['output']['alt'],
				ioData['output']['gimbalRoll'],
				ioData['output']['gimbalPitch']
			)
			ser.write(m)
			# print m

def getNormalalizedAxisValue(device,axisNo):
	return float(pad.get_axis(axisNo))

def limitTo(val,val_min=-1,val_max=1):
	return min(val_max,max(val_min,val))

def readUserInput(events):
	global isAppRunning
	global ioData
	for event in events:
		if event.type == pygame.QUIT or (event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE):
			isAppRunning = False
		elif event.type == pygame.JOYHATMOTION:
			if event.value[1] == 1:
				ioData['output']['gimbalPitch'] = limitTo(ioData['output']['gimbalPitch'] + 0.1)
			elif event.value[1] == -1:
				ioData['output']['gimbalPitch'] = limitTo(ioData['output']['gimbalPitch'] - 0.1)
			elif event.value[0] == 1:
				ioData['output']['gimbalRoll'] = limitTo(ioData['output']['gimbalRoll'] + 0.1)
			elif event.value[0] == -1:
				ioData['output']['gimbalRoll'] = limitTo(ioData['output']['gimbalRoll'] - 0.1)
		# For debug
		# elif event.type == pygame.KEYDOWN and event.key == pygame.K_UP:
		# 	ioData['input']['pitch'] = limitTo(ioData['input']['pitch'] + 0.01)
		# elif event.type == pygame.KEYDOWN and event.key == pygame.K_DOWN:
		# 	ioData['input']['pitch'] = limitTo(ioData['input']['pitch'] - 0.01)
		# elif event.type == pygame.KEYDOWN and event.key == pygame.K_LEFT:
		# 	ioData['output']['pitch'] = limitTo(ioData['output']['pitch'] + 0.01)
		# elif event.type == pygame.KEYDOWN and event.key == pygame.K_RIGHT:
		# 	ioData['output']['pitch'] = limitTo(ioData['output']['pitch'] - 0.01)
		# else:
		# 	print event
		# y = limitTo(getNormalalizedAxisValue(pad,0))
		# r = limitTo(getNormalalizedAxisValue(pad,3))
		# p = limitTo(getNormalalizedAxisValue(pad,2))
		# t = limitTo(getNormalalizedAxisValue(pad,1))
		
		# ioData['output']['gimbalRoll'] = limitTo(getNormalalizedAxisValue(pad,3))
		# ioData['output']['gimbalPitch'] = limitTo(getNormalalizedAxisValue(pad,2))

		# Przyciski
		if pad != None:
			if pad.get_button(9): # Wyjscie z programu
				isAppRunning = False

			keys2mode = {
				0 : 'IDLE',
				7 : 'GIMBAL',
				5 : 'MOTOR_TEST'
			}
			for keyNo,modeStr in keys2mode.items():
				if pad.get_button(keyNo):
					ioData['output']['msgs'].append('m%s'%modeStr)

horizontRoll = pygame.image.load('horizontRoll.png')
horizontPitch =pygame.image.load('horizontPitch.png')
horizontBackground =pygame.image.load('horizontBackground.png')
horizontTarget =pygame.image.load('horizontTarget.png')
horizontFont = pygame.font.SysFont("monospace", 12)
def renderBalance(x,y,value,target,image):
	screen.blit(horizontBackground, (x-(horizontBackground.get_width()/2), y-(horizontBackground.get_height()/2)))
	targetRotate = pygame.transform.rotate(horizontTarget, target * 180)
	screen.blit(targetRotate, (x-(targetRotate.get_width()/2), y-(targetRotate.get_height()/2)))
	valueRotated = pygame.transform.rotate(image, value * 180)
	screen.blit(valueRotated, (x-(valueRotated.get_width()/2), y-(valueRotated.get_height()/2)))
	descValue = horizontFont.render("%f"%value, 1, (255,255,128))
	descTarget = horizontFont.render("%f"%target, 1, (255,128,128))
	screen.blit(descTarget, (x-(descTarget.get_width()/2), y-(horizontBackground.get_height()/2)-descTarget.get_height()))
	screen.blit(descValue, (x-(descValue.get_width()/2), y+(horizontBackground.get_height()/2)))


def appGui():
	global isAppRunning
	global ioData
	myfont = pygame.font.SysFont("monospace", 12)
	while isAppRunning:
		# GUI drawing
		clock.tick(60)
		readUserInput(pygame.event.get())

		screen.blit(background, (0,0))

		renderBalance(266,200,ioData['input']['roll'],ioData['output']['roll'],horizontRoll)
		renderBalance(533,200,ioData['input']['pitch'],ioData['output']['pitch'],horizontPitch)

		lpsText = myfont.render("LPS: %f"%(ioData['input']['lps']*10), 1, (255,255,0))
		screen.blit(lpsText, (200, 20))
		
		pygame.display.flip()

# try:
ser = serial.Serial(
	port='/dev/ttyUSB0',\
	baudrate=57600,\
	parity=serial.PARITY_NONE,\
	stopbits=serial.STOPBITS_ONE,\
	bytesize=serial.EIGHTBITS,\
	timeout=None
)
pygame.joystick.init()
pad = pygame.joystick.Joystick(0)
pad.init()
# except Exception, e:
# 	print "Serial nie podlaczony!!!"
# 	ser = None
# 	pad = None

try:
	if ser != None:
		thread.start_new_thread( communicationInput, (ser,) )
		thread.start_new_thread( communicationOutput, (ser,) )
	thread.start_new_thread( appGui, () )
except:
	print "Error: unable to start thread"

while isAppRunning:
	time.sleep(.1)