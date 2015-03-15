#include "ioWrapper.h"

#include "miniCopterPro.h"
#include "autoPilot.h"

void ioWrapper::sendStatus(){
	IO_SERIAL_STREAM.print('<');
	IO_SERIAL_STREAM.print(((miniCopterPro*)copterPointer)->wd.getLPS());
	IO_SERIAL_STREAM.print(',');
	IO_SERIAL_STREAM.print(((miniCopterPro*)copterPointer)->wd.getLPS());
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
	switch(cmd[0]){
		case 'g': case 'G':/* Gimball */
			switch(cmd[1]){ /* axis */
				case 'x': case 'X':
					((miniCopterPro*)copterPointer)->setGimbalTarget(0 , ((float)(cmd[2]-'1'))/5.f-1);
					break;
				case 'y':
				case 'Y':
					((miniCopterPro*)copterPointer)->setGimbalTarget(1 , ((float)(cmd[2]-'1'))/5.f-1);
					break;
			}
			break;
		case 'p': case 'P':/* Pilot */
			switch(cmd[1]){ /* axis */
				case 'm': case 'M':
					switch(cmd[2]){
						case 'g': case 'G': 
							((miniCopterPro*)copterPointer)->pilot.setMode(PILOT_MODE_GIMBAL_RUN);
							break;
						case 'i': case 'I': 
							((miniCopterPro*)copterPointer)->pilot.setMode(PILOT_MODE_IDLE);
							break;
						case 'm': case 'M': 
							((miniCopterPro*)copterPointer)->pilot.setMode(PILOT_MODE_MOTOR_TEST);
							break;
					}
					break;
			}
			break;
	}
}
void ioWrapper::update(){
	static char inBuff[16];
	static uint8_t inBuffPos=0;
	while(IO_SERIAL_STREAM.available()){
		inBuff[inBuffPos] = IO_SERIAL_STREAM.read();
		IO_SERIAL_STREAM.print("> ");
		IO_SERIAL_STREAM.println(inBuff[inBuffPos]);
		if(inBuffPos==0){
			if(inBuff[inBuffPos]=='<')
				inBuffPos++;
		} else {
			if(inBuff[inBuffPos] == '>'){
				inBuff[inBuffPos] = '\0';
				runCommand(inBuff+1,inBuffPos-1);
				inBuffPos = 0;
			} else {
				if(inBuffPos >= 16)
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