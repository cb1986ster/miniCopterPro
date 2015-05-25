PROJECT_DIR = $(CURDIR)
USER_LIB_PATH := $(PROJECT_DIR)/lib
ARDUINO_LIBS += I2Cdev NewPing Servo mpu SFE_BMP180 freeram
ARCHITECTURE  = avr
BOARD_TAG = nano
BOARD_SUB = atmega328
MCU = atmega328p

MONITOR_BAUDRATE = 57600
AVRDUDE_ARD_BAUDRATE = 57600 
MONITOR_PORT = /dev/ttyUSB0
#-Wl,-u,vfscanf -lscanf_flt -lm 
CXXFLAGS += -D MPU9150 -Wl,-u,vfscanf -lscanf_flt -lm 

#########################################################
## Reszty nie ruszać
ARDUINO_DIR = /opt/arduino-1.5.7
AVR_TOOLS_DIR = /usr
# AVR_TOOLS_DIR = /opt/arduino-1.5.7/hardware/tools/avr
AVRDDUDE = /usr/bin/avrdude
# AVRDDUDE = /opt/arduino-1.5.7/hardware/tools/avr/bin/avrdude
ARDMK_DIR = /opt/Arduino-Makefile
#########################################################
## A tego to już w szczególności
### CFLAGS_STD
### Set the C standard to be used during compilation.
### Documentation (https://github.com/WeAreLeka/Arduino-Makefile/blob/std-flags/arduino-mk-vars.md#cflags_std)
CFLAGS_STD = -std=gnu11
### CXXFLAGS_STD
### Set the C++ standard to be used during compilation.
### Documentation (https://github.com/WeAreLeka/Arduino-Makefile/blob/std-flags/arduino-mk-vars.md#cxxflags_std)
CXXFLAGS_STD = -std=gnu++11
### CXXFLAGS
### Flags you might want to set for debugging purpose. Comment to stop.
#CXXFLAGS += -pedantic -Wall -Wextra
### CURRENT_DIR
### Do not touch - used for binaries path
CURRENT_DIR = $(shell basename $(CURDIR))
### OBJDIR
### This is were you put the binaries you just compile using 'make'
OBJDIR = $(PROJECT_DIR)/bin/$(BOARD_TAG)/$(CURRENT_DIR)
### path to Arduino.mk, inside the ARDMK_DIR, don't touch.
include $(ARDMK_DIR)/Arduino.mk