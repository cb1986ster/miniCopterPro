#include "ioWrapper.h"

#include "miniCopterPro.h"
#include "autoPilot.h"
#include <ctype.h>

#include <ctype.h>

float my_atof(char *s)
{
	float f = 0.0, fd = 0.0, d = 0.0;
	int fe = 0;
	char negative = 0;

	while (isspace(*s) && *s != 0)
		s++;

	if (*s == '+')
		s++;

	if (*s == '-') {
		s++;
		negative = 1;
	}

	while (isdigit(*s) && *s != 0) {
		f  *= 10;
		f += (*s - '0');
		s++;
	}
	if (*s == '.') {
		s++;
		d = 1.0;
		while (isdigit(*s) && *s != 0) {
			fd *= 10;
			d *= 10;
			fd += (*s - '0');
			s++;
		}
		fd /= d;
	}

	f += fd;
	if (negative)
		f = -f;

	return f;
}

void ioWrapper::sendStatus(){
	IO_SERIAL_STREAM.write('<');
	writeValue( ((miniCopterPro*)copterPointer)->sensors.getPitch());
	sendSeparator();
	writeValue( ((miniCopterPro*)copterPointer)->sensors.getRoll());
	sendSeparator();
	writeValue( ((miniCopterPro*)copterPointer)->wd.getLPS());
	sendSeparator();
	writeValue( ((miniCopterPro*)copterPointer)->getGimbalTarget(0));
	sendSeparator();
	writeValue( ((miniCopterPro*)copterPointer)->getGimbalTarget(1));

	sendSeparator();
	writeValue( ((miniCopterPro*)copterPointer)->effectors.getMotorSpeed(0));
	sendSeparator();
	writeValue( ((miniCopterPro*)copterPointer)->effectors.getMotorSpeed(1));
	sendSeparator();
	writeValue( ((miniCopterPro*)copterPointer)->effectors.getMotorSpeed(2));
	sendSeparator();
	writeValue( ((miniCopterPro*)copterPointer)->effectors.getMotorSpeed(3));

	IO_SERIAL_STREAM.write('>');
	lineEnd();
}

void ioWrapper::init(){
	IO_SERIAL_STREAM.begin(IO_SERIAL_BAUD);
	lineBegin();
	sendMesg(ioText_ioInited);
	lineEnd();
}

void ioWrapper::sendMesg(const char* msg){
	lineBegin();
	IO_SERIAL_STREAM.write(msg);
	lineEnd();
}
void ioWrapper::sendMesg(const char* msg,int value){
	char obuf[IO_MAX_MSG_LEN];
	sprintf(obuf,msg,value);
	lineBegin();
	IO_SERIAL_STREAM.write(obuf);
	lineEnd();
}
void ioWrapper::sendMesgNoNl(const char* msg){
	lineBegin();
	IO_SERIAL_STREAM.write(msg);
}
void ioWrapper::sendMesgNoStart(const char* msg){
	IO_SERIAL_STREAM.write(msg);
}
void ioWrapper::runCommand(char* cmd,uint8_t len){
	static float tempFloat[6]={0,0,0,0,0,0};
	static uint8_t i,pos_b,pos_e;
	// IO_SERIAL_STREAM.print('@');
	// IO_SERIAL_STREAM.println(cmd);
	switch(cmd[0]){
		case 'p': case 'P': /* GetParams */
			pos_b = 1;
			pos_e = 2;
			cmd[len] = ':';
			for(i=0;i<6;){
				if(cmd[pos_e]==':'){
					cmd[pos_e] = 0;
					tempFloat[i] = my_atof(cmd+pos_b);					
					pos_b = pos_e+1;
					pos_e = pos_b+1;
					i++;
				} else
					pos_e++;
			}
			((miniCopterPro*)copterPointer)->setPlatformTarget(0,tempFloat[0]);
			((miniCopterPro*)copterPointer)->setPlatformTarget(1,tempFloat[1]);
			((miniCopterPro*)copterPointer)->setRotationTarget(tempFloat[2]);
			((miniCopterPro*)copterPointer)->setAltChangeTarget(tempFloat[3]);

			((miniCopterPro*)copterPointer)->setGimbalTarget(0 , tempFloat[4]);
			((miniCopterPro*)copterPointer)->setGimbalTarget(1 , tempFloat[5]);
			break;
		case 'm': case 'M':
			switch(cmd[1]){
				case 'I': case 'i':
					((miniCopterPro*)copterPointer)->pilot.setMode(PILOT_MODE_IDLE);
					break;
				case 'G': case 'g':
					((miniCopterPro*)copterPointer)->pilot.setMode(PILOT_MODE_GIMBAL_RUN);
					break;
				case 'T': case 't':
					((miniCopterPro*)copterPointer)->pilot.setMode(PILOT_MODE_MOTOR_TEST);
					break;
				case 'L': case 'l':
					((miniCopterPro*)copterPointer)->pilot.setMode(PILOT_MODE_LANDING);
					break;
				case 'S': case 's':
					((miniCopterPro*)copterPointer)->pilot.setMode(PILOT_MODE_START);
					break;
				case 'H': case 'h':
					((miniCopterPro*)copterPointer)->pilot.setMode(PILOT_MODE_STABLISATION);
					break;
				case 'F': case 'f':
					((miniCopterPro*)copterPointer)->pilot.setMode(PILOT_MODE_FLY);
					break;
			}
			break;
	}
}
#define inBuffMaxLen 56
void ioWrapper::update(){
	static char inBuff[inBuffMaxLen];
	static uint8_t inBuffPos=0;
	while(IO_SERIAL_STREAM.available()){
		inBuff[inBuffPos] = IO_SERIAL_STREAM.read();
		if(inBuffPos==0){
			if(inBuff[inBuffPos]=='<')
				inBuffPos++;
		} else {
			if(inBuff[inBuffPos] == '>'){
				inBuff[inBuffPos] = '\0';
				runCommand(inBuff+1,inBuffPos-1);
				inBuffPos = 0;
			} else {
				if(inBuffPos >= inBuffMaxLen)
					inBuffPos = 0;
				else 	
					inBuffPos++;
			}
		}
	}
}
void ioWrapper::triggerCriticalError(){
	while(1);
}