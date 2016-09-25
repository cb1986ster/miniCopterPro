from communication import dronePort
import sys

if len(sys.argv) != 5:
    print 'usage: %s pidNo Kp Ki Kd'%sys.argv[0]
    exit (1)

dronePort.paramsSend('T',sys.argv[1:])
