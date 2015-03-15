import pygame, sys,os, math
import thread
import time
import serial
import re

from pygame.locals import *

pygame.init()

window = pygame.display.set_mode((800, 600))
pygame.display.set_caption('QuadCopter controll')
background = pygame.image.load('background.png').convert()
horizontRoll = pygame.image.load('horizontRoll.png')
horizontPitch =pygame.image.load('horizontPitch.png')

roll = 0
pitch = 0
lps = 0

screen = pygame.display.get_surface()

clock= pygame.time.Clock()

isAppRunning = True

def input(events):
	global isAppRunning
	for event in events:
		if event.type == QUIT:
			isAppRunning = False
		else:
			print event
def appSerialReader():
	global isAppRunning
	global pitch
	global roll
	global lps
	ser = serial.Serial(
		port='/dev/rfcomm0',\
		baudrate=57600,\
		parity=serial.PARITY_NONE,\
		stopbits=serial.STOPBITS_ONE,\
		bytesize=serial.EIGHTBITS,\
		timeout=1
	)
	while isAppRunning:
		line = ser.readline()
		data = re.findall(r"\<([-+]?\d*\.\d+|\d+),([-+]?\d*\.\d+|\d+),([-+]?\d*\.\d+|\d+)\>", line)
		if len(data) > 0:
			data = data[-1]
			pitch = float(data[0])
			roll = float(data[1])
			llps = float(data[2])
			lps = llps*0.05 + lps*0.95

def appGui():
	global pitch
	global roll
	global isAppRunning
	global lps
	myfont = pygame.font.SysFont("monospace", 15)
	while isAppRunning:
		clock.tick(60)
		input(pygame.event.get())
		screen.blit(background, (0,0))
		rollRot = pygame.transform.rotate(horizontRoll, roll * 180)
		screen.blit(rollRot, (266-(rollRot.get_width()/2), 200-(rollRot.get_height()/2)))
		pitchRot =pygame.transform.rotate(horizontPitch, pitch * 180)
		screen.blit(pitchRot, (533-(pitchRot.get_width()/2), 200-(pitchRot.get_height()/2)))
		lpsText = myfont.render("LPS: %f"%(lps*10), 1, (255,255,0))
		screen.blit(lpsText, (200, 20))
		pygame.display.flip()

try:
	thread.start_new_thread( appSerialReader, () )
	thread.start_new_thread( appGui, () )
except:
	print "Error: unable to start thread"

while isAppRunning:
	time.sleep(1)