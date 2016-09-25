#include "ioWrapper.h"

#include "miniCopterPro.h"
#include "autoPilot.h"
#include <ctype.h>

#include "my_atof.c"

#define copter ((miniCopterPro*)copterPointer)

// Helpers
void sendSeparator() {IO_SERIAL_STREAM.write(',');};
void lineBegin() {IO_SERIAL_STREAM.write('|');};
void lineEnd() {IO_SERIAL_STREAM.write('\n');};
void unpackFloats(char* str,int pos_b,int pos_e,int len,float target[],int slen){
	static uint8_t i=0;
	str[slen] = ':';
	for(i=0;i<len;){
		if(str[pos_e]==':'){
			str[pos_e] = 0;
			target[i] = my_atof(str+pos_b);
			pos_b = pos_e+1;
			pos_e = pos_b+1;
			i++;
		} else
			pos_e++;
	}
}
// void my_itoa_10(int z,char *buffer){
// 	uint32_t p=1000000;
// 	for(;z/p==0;p/=10); // looking for row
// 	for(;p!=1;p/=10,buffer++)*buffer='0'+((z/p)%10);
// 	*(buffer+1)=0; // terminator
// }

void writeValue(float val){
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
// implem
void ioWrapper::sendStatus(){
	IO_SERIAL_STREAM.write('<');
	writeValue( copter->sensors.getPitch());
	sendSeparator();
	writeValue( copter->sensors.getRoll());
	sendSeparator();
	writeValue( copter->wd.getLPS());
	sendSeparator();
	writeValue( copter->effectors.getMotorSpeed(0));
	sendSeparator();
	writeValue( copter->effectors.getMotorSpeed(1));
	sendSeparator();
	writeValue( copter->effectors.getMotorSpeed(2));
	sendSeparator();
	writeValue( copter->effectors.getMotorSpeed(3));

	sendSeparator();
	writeValue( copter->sensors.getRotation());

	sendSeparator();
	writeValue( copter->sensors.getBatteryStatus());

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
	static uint8_t i;
	switch(cmd[0]){
		case 'T': /* GetParams */ // PID tunning
			unpackFloats(cmd,1,2,4,tempFloat,len);
			copter->pilot.tunePID(tempFloat[0],(double*)(tempFloat+1));
			break;
		case 'P': /* GetParams */ // Fly params - roll, yaw, pitch ect
			unpackFloats(cmd,1,2,6,tempFloat,len);
			copter->setPlatformTarget(0,tempFloat[0]);
			copter->setPlatformTarget(1,tempFloat[1]);
			copter->setRotationTarget(tempFloat[2]);
			copter->setAltChangeTarget(tempFloat[3]);

			copter->setGimbalTarget(0 , tempFloat[4]);
			copter->setGimbalTarget(1 , tempFloat[5]);
			break;
		case 'E': /* GetParams */ // Recive eeprom related message
			switch(cmd[1]){
				case 'I': // Write something
					unpackFloats(cmd,2,3,2,tempFloat,len);
					copter->pilot.saveInEEPROM(int(tempFloat[0]),int(tempFloat[1]));
					break;
				case 'O': // Read something
					i = my_atof(cmd+2);
					IO_SERIAL_STREAM.write('<');
					IO_SERIAL_STREAM.write('E');
					writeValue(i);
					sendSeparator();
					i = copter->pilot.getEEPROM(i);
					writeValue(i);
					IO_SERIAL_STREAM.write('>');
					lineEnd();
					break;
			}
			break;
		case 'M': // Recive simple message - no params
			switch(cmd[1]){
				case 'R': // DANGER!!!
					copter->wd.reset();
					break;
				case 'I':
					copter->pilot.setMode(PILOT_MODE_IDLE);
					break;
				case 'G':
					copter->pilot.setMode(PILOT_MODE_GIMBAL_RUN);
					break;
				case 'T':
					copter->pilot.setMode(PILOT_MODE_MOTOR_TEST);
					break;
				case 'L':
					copter->pilot.setMode(PILOT_MODE_LANDING);
					break;
				case 'S':
					copter->pilot.setMode(PILOT_MODE_START);
					break;
				case 'H':
					copter->pilot.setMode(PILOT_MODE_STABLISATION);
					break;
				case 'F':
					copter->pilot.setMode(PILOT_MODE_FLY);
					break;
				case 'D': // Dance mode
					copter->pilot.setMode(PILOT_MODE_DANCE);
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
		if(inBuffPos==0 && inBuff[inBuffPos]=='<'){
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
