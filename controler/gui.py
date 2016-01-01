import pygame, sys,os, math
import thread
import time
import serial
import re
from communication import *

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
		'rotate':0.0,
		'lps':0.0,
		'gimbalRoll':0.0,
		'gimbalPitch':0.0,
		'motors':[ 0.0, 0.0, 0.0, 0.0]
	}
}

pygame.init()
window = None
if False:
	window = pygame.display.set_mode((800, 600),pygame.FULLSCREEN)
else:
	window = pygame.display.set_mode((800, 600))
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
		data = re.findall(r"\<([-+]?\d*\.\d+|\d+),([-+]?\d*\.\d+|\d+),([-+]?\d*\.\d+|\d+),([-+]?\d*\.\d+|\d+),([-+]?\d*\.\d+|\d+),([-+]?\d*\.\d+|\d+),([-+]?\d*\.\d+|\d+),([-+]?\d*\.\d+|\d+),([-+]?\d*\.\d+|\d+),([-+]?\d*\.\d+|\d+)\>", line)
		if len(data) > 0:
			ioData['input']['pitch'] = float(data[-1][0])
			ioData['input']['roll'] = float(data[-1][1])
			ioData['input']['lps'] = ioData['input']['lps']*0.9 + float(data[-1][2])*0.1
			ioData['input']['gimbalRoll'] = float(data[-1][3])
			ioData['input']['gimbalPitch'] = float(data[-1][4])
			# motors
			ioData['input']['motors'][0] = float(data[-1][5])
			ioData['input']['motors'][1] = float(data[-1][6])
			ioData['input']['motors'][2] = float(data[-1][7])
			ioData['input']['motors'][3] = float(data[-1][8])
			# rotacja
			ioData['input']['rotate'] = ioData['input']['rotate']*0.7 + float(data[-1][9])*0.3

def communicationOutput(ser):
	global isAppRunning
	global ioData
	i = 0
	while isAppRunning:
		time.sleep(.001)
		while len(ioData['output']['msgs']) > 0:
			m = ioData['output']['msgs'].pop()
			ser.write("<%s>"%m)
			print '                  sending: %s'%m

		i+=1
		if i > 25:
			i=0
			m = "<P%0.3f:%0.3f:%0.3f:%0.3f:%0.3f:%0.3f>"%(
				ioData['output']['roll'],
				ioData['output']['pitch'],
				ioData['output']['rotate'],
				ioData['output']['alt'],
				ioData['output']['gimbalRoll'],
				ioData['output']['gimbalPitch']
			)
			ser.write(m)
			# print '          ','          ',m

def getNormalalizedAxisValue(device,axisNo):
	if device == None:
		return 0.0
	v = float(device.get_axis(axisNo))/2.0
	return v

def limitTo(val,val_min=-1,val_max=1):
	return min(val_max,max(val_min,val))

def readUserInput(events):
	global isAppRunning
	global ioData
	global y
	global x
	global t
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

		# t = limitTo(getNormalalizedAxisValue(pad,1))

		# ioData['output']['gimbalRoll'] = limitTo(getNormalalizedAxisValue(pad,3))
		# ioData['output']['gimbalPitch'] = limitTo(getNormalalizedAxisValue(pad,2))

		# Przyciski
		if pad != None:
			ioData['output']['roll'] = -limitTo(getNormalalizedAxisValue(pad,0))
			ioData['output']['pitch'] = limitTo(getNormalalizedAxisValue(pad,2))
			ioData['output']['rotate'] = -limitTo(getNormalalizedAxisValue(pad,3))
			print ioData['output']['roll'], ioData['output']['pitch'],ioData['output']['rotate']
			if pad.get_button(9): # Wyjscie z programu
				isAppRunning = False
			keys2mode = {
				0 : 'I',
				4 : 'D',
				7 : 'G',
				5 : 'T'
			}
			for keyNo,modeStr in keys2mode.items():
				if pad.get_button(keyNo):
					ioData['output']['msgs'].append('M%s'%modeStr)

rotationValue =pygame.image.load('rotation.png')
rotationBackground =pygame.image.load('horizontBackground.png')
rotationTarget =pygame.image.load('rotationTarget.png')
rotationFont = pygame.font.SysFont("monospace", 15)

horizontRoll = pygame.image.load('horizontRoll.png')
horizontPitch =pygame.image.load('horizontPitch.png')
horizontBackground =pygame.image.load('horizontBackground.png')
horizontTarget =pygame.image.load('horizontTarget.png')
horizontFont = pygame.font.SysFont("monospace", 15)


def gradGreenYellowRed(v):
	if v>0.5:
		r = 255
		g = int(255*(1.0-v)/0.5)
	else:
		g = 255
		r = int(512*v)
	return (r,g,16)

def renderBalance(x,y,value,target,image):
	screen.blit(horizontBackground, (x-(horizontBackground.get_width()/2), y-(horizontBackground.get_height()/2)))
	targetRotate = pygame.transform.rotozoom(horizontTarget, target * 180, 1)
	screen.blit(targetRotate, (x-(targetRotate.get_width()/2), y-(targetRotate.get_height()/2)))
	valueRotated = pygame.transform.rotozoom(image, value * 180, 1)
	screen.blit(valueRotated, (x-(valueRotated.get_width()/2), y-(valueRotated.get_height()/2)))
	descValue = horizontFont.render("%f"%value, 1, (255,255,128))
	descTarget = horizontFont.render("%f"%target, 1, (255,128,128))
	screen.blit(descTarget, (x-(descTarget.get_width()/2), y-(horizontBackground.get_height()/2)-descTarget.get_height()))
	screen.blit(descValue, (x-(descValue.get_width()/2), y+(horizontBackground.get_height()/2)))

def renderRotate(x,y,value,target):
	screen.blit(rotationBackground, (x-(rotationBackground.get_width()/2), y-(rotationBackground.get_height()/2)))
	targetRotate = pygame.transform.rotozoom(rotationTarget, target * -1.0, 1)
	screen.blit(targetRotate, (x-(targetRotate.get_width()/2), y-(targetRotate.get_height()/2)))
	valueRotated = pygame.transform.rotozoom(rotationValue, value * -1.0, 1)
	screen.blit(valueRotated, (x-(valueRotated.get_width()/2), y-(valueRotated.get_height()/2)))
	descValue = rotationFont.render("%f"%value, 1, (255,255,128))
	descTarget = rotationFont.render("%f"%target, 1, (255,128,128))
	screen.blit(descTarget, (x-(descTarget.get_width()/2), y-(rotationBackground.get_height()/2)-descTarget.get_height()))
	screen.blit(descValue, (x-(descValue.get_width()/2), y+(rotationBackground.get_height()/2)))

def renderMotorSpeed(x,y,value):
	value = value/255.0
	msx = 40
	msy = 80
	pygame.draw.rect(screen,(64,64,255),(x-(msx/2),y-(msy/2),msx,msy),0)
	height = int((msy-10)*value)
	offset = int((1-value)*(msy-10))
	pygame.draw.rect(screen,gradGreenYellowRed(value),(5+x-(msx/2),offset+5+y-(msy/2),msx-10,height),0)
	descValue = rotationFont.render("%d%%"%(100*value), 1, (255,255,128))
	screen.blit(descValue, (x-(descValue.get_width()/2), y+(msy/2)))

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
		renderRotate(533,420,ioData['input']['rotate'],ioData['output']['rotate'])

		renderMotorSpeed(30,520, ioData['input']['motors'][0])
		renderMotorSpeed(770,50, ioData['input']['motors'][1])
		renderMotorSpeed(770,520, ioData['input']['motors'][2])
		renderMotorSpeed(30,50, ioData['input']['motors'][3])


		lpsText = myfont.render("LPS: %f"%(ioData['input']['lps']*10), 1, (255,255,0))
		screen.blit(lpsText, (200, 20))

		pygame.display.flip()

# try:
ser = dronePort
pygame.joystick.init()
pad = None
try:
	pad = pygame.joystick.Joystick(0)
	pad.init()
except Exception, e:
	print "Bez padu ;("
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
	time.sleep(.05)
