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
void ioWrapper::writeValue(float val){
#if SERIAL_WRITE_NATIVE_FLOAT
	static char buffer[16];
	dtostrf(val,1,3,buffer);
	IO_SERIAL_STREAM.write(buffer);
#else
	#if SERIAL_WRITE_FLOAT_IMP_NO == 0
		static int z,i;
		static char buffer[16];
		if(val < 0.0){
			IO_SERIAL_STREAM.write('-');
			val *= -1;
		}
		if(val > 999){
			z = (val)/1000;
			itoa(z,buffer,10);
			IO_SERIAL_STREAM.write(buffer);
			val -= z*1000;
		}
		z = (val)/100;
		buffer[0] = ('0' + z);
		val -= z*100;

		z = (val)/10;
		buffer[1] = ('0' + z);
		val -= z*10;

		z = (val);
		buffer[2] =('0' + z);
		val -= z;

		buffer[3] = '.';
		// IO_SERIAL_STREAM.write(buffer);
		for(i=0;i<4;i++){
			val *= 10;
			z = (val);
			buffer[i+4] = '0'+ (z%10);
			val -= z;
		}
		buffer[8]=0;
		IO_SERIAL_STREAM.write(buffer);
	#endif
	#if SERIAL_WRITE_FLOAT_IMP_NO == 1
		static int z;
		static char buffer[16];
		if(val < 0.0){
			IO_SERIAL_STREAM.write('-');
			val *= -1;
		}
		z = int(val);
		itoa(z,buffer,10);
		IO_SERIAL_STREAM.write(buffer);
		IO_SERIAL_STREAM.write('.');
		z = (val-z)*1000;
		if(z<100){
			IO_SERIAL_STREAM.write('0');
			if(z<10){
				IO_SERIAL_STREAM.write('0');
				if(z<1){
					IO_SERIAL_STREAM.write('0');
				}
			}
		}
		itoa(z,buffer,10);
		IO_SERIAL_STREAM.write(buffer);
	#endif
#endif
};
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
	sendSeparator();
	writeValue( ((miniCopterPro*)copterPointer)->sensors.getRotation());

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
	IO_SERIAL_STREAM.write(msg,strlen(msg));
	lineEnd();
}
void ioWrapper::sendMesg(const char* msg,int value){
	char obuf[IO_MAX_MSG_LEN];
	sprintf(obuf,msg,value);
	lineBegin();
	IO_SERIAL_STREAM.write(obuf,strlen(obuf));
	lineEnd();
}
void ioWrapper::sendMesgNoNl(const char* msg){
	lineBegin();
	IO_SERIAL_STREAM.write(msg,strlen(msg));
}
void ioWrapper::sendMesgNoStart(const char* msg){
	IO_SERIAL_STREAM.write(msg,strlen(msg));
}
void ioWrapper::runCommand(char* cmd,uint8_t len){
	static float tempFloat[6]={0,0,0,0,0,0};
	static uint8_t i,pos_b,pos_e;
	// IO_SERIAL_STREAM.print('@');
	// IO_SERIAL_STREAM.println(cmd);
	switch(cmd[0]){
		case 'P': /* GetParams */
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
		case 'E': /* GetParams */
			switch(cmd[1]){
				case 'I':
					pos_b = 2;
					pos_e = 3;
					cmd[len] = ':';
					for(i=0;i<2;){
						if(cmd[pos_e]==':'){
							cmd[pos_e] = 0;
							tempFloat[i] = my_atof(cmd+pos_b);					
							pos_b = pos_e+1;
							pos_e = pos_b+1;
							i++;
						} else
							pos_e++;
					}
					((miniCopterPro*)copterPointer)->pilot.saveInEEPROM(int(tempFloat[0]),int(tempFloat[1]));
					break; 
				case 'O':
					pos_b = my_atof(cmd+2);
					pos_e = ((miniCopterPro*)copterPointer)->pilot.getEEPROM(pos_b);
					IO_SERIAL_STREAM.write('<');
					IO_SERIAL_STREAM.write('E');
					writeValue(pos_b);
					sendSeparator();
					writeValue(pos_e);
					IO_SERIAL_STREAM.write('>');
					lineEnd();
					break;
			}
			break;
		case 'M':
			switch(cmd[1]){
				case 'I':
					((miniCopterPro*)copterPointer)->pilot.setMode(PILOT_MODE_IDLE);
					break;
				case 'G':
					((miniCopterPro*)copterPointer)->pilot.setMode(PILOT_MODE_GIMBAL_RUN);
					break;
				case 'T':
					((miniCopterPro*)copterPointer)->pilot.setMode(PILOT_MODE_MOTOR_TEST);
					break;
				case 'L':
					((miniCopterPro*)copterPointer)->pilot.setMode(PILOT_MODE_LANDING);
					break;
				case 'S':
					((miniCopterPro*)copterPointer)->pilot.setMode(PILOT_MODE_START);
					break;
				case 'H':
					((miniCopterPro*)copterPointer)->pilot.setMode(PILOT_MODE_STABLISATION);
					break;
				case 'F':
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