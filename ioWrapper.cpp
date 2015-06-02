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
	IO_SERIAL_STREAM.print('<');
	IO_SERIAL_STREAM.print(((miniCopterPro*)copterPointer)->sensors.getPitch());
	IO_SERIAL_STREAM.print(',');
	IO_SERIAL_STREAM.print(((miniCopterPro*)copterPointer)->sensors.getRoll());
	IO_SERIAL_STREAM.print(',');
	IO_SERIAL_STREAM.print(((miniCopterPro*)copterPointer)->wd.getLPS());
	IO_SERIAL_STREAM.print(',');
	IO_SERIAL_STREAM.print(((miniCopterPro*)copterPointer)->getGimbalTarget(0));
	IO_SERIAL_STREAM.print(',');
	IO_SERIAL_STREAM.print(((miniCopterPro*)copterPointer)->getGimbalTarget(1));
	IO_SERIAL_STREAM.println('>');
}

void ioWrapper::init(){
	IO_SERIAL_STREAM.begin(IO_SERIAL_BAUD);
	for(int i = IO_MAX_MSG_LEN-1;i;i--)buffer[i]=0;
	buffer[0]='|';
	sendMesg(ioText_ioInited);
}

void ioWrapper::sendMesg(const char* msg){
	strcpy_P(buffer+1,msg);
	IO_SERIAL_STREAM.println(buffer);
}
void ioWrapper::sendMesg(const char* msg,int value){
	char obuf[IO_MAX_MSG_LEN+5];
	strcpy_P(buffer+1,msg);
	sprintf(obuf,buffer,value);
	IO_SERIAL_STREAM.println(obuf);
}
void ioWrapper::sendMesgNoNl(const char* msg){
	strcpy_P(buffer+1,msg);
	IO_SERIAL_STREAM.print(buffer);
}
void ioWrapper::sendMesgNoStart(const char* msg){
	strcpy_P(buffer+1,msg);
	IO_SERIAL_STREAM.print(buffer+1);
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
			((miniCopterPro*)copterPointer)->setPlatformTarget(tempFloat[0],tempFloat[1]);
			((miniCopterPro*)copterPointer)->setRotationTarget(tempFloat[2]);
			((miniCopterPro*)copterPointer)->setAltChangeTarget(tempFloat[3]);

			((miniCopterPro*)copterPointer)->setGimbalTarget(0 , tempFloat[4]);
			((miniCopterPro*)copterPointer)->setGimbalTarget(1 , tempFloat[5]);
			break;
		case 'm': case 'M':
			if(strncmp(cmd+1,"IDLE",4)==0){
				((miniCopterPro*)copterPointer)->pilot.setMode(PILOT_MODE_IDLE);
			} else if(strncmp(cmd+1,"GIMBAL",6)==0){
				((miniCopterPro*)copterPointer)->pilot.setMode(PILOT_MODE_GIMBAL_RUN);
			} else if(strncmp(cmd+1,"MOTOR_TEST",10)==0){
				((miniCopterPro*)copterPointer)->pilot.setMode(PILOT_MODE_MOTOR_TEST);
			} else if(strncmp(cmd+1,"LAND",4)==0){
				((miniCopterPro*)copterPointer)->pilot.setMode(PILOT_MODE_LANDING);
			} else if(strncmp(cmd+1,"START",5)==0){
				((miniCopterPro*)copterPointer)->pilot.setMode(PILOT_MODE_START);
			} else if(strncmp(cmd+1,"HOLD",4)==0){
				((miniCopterPro*)copterPointer)->pilot.setMode(PILOT_MODE_STABLISATION);
			} else if(strncmp(cmd+1,"FLY",3)==0){
				((miniCopterPro*)copterPointer)->pilot.setMode(PILOT_MODE_FLY);
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