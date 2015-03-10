#ifndef _IOWRAPPER_H_
#define _IOWRAPPER_H_ 1

#include <avr/pgmspace.h> /* For PROGMEM */


#define IO_MAX_MSG_LEN 19
/* GLOBALS !!! */
const char ioText_ioInited[] PROGMEM = "IO init successful. Starting :)";
const char ioText_sensorsInit[] PROGMEM = "Sensors initing ";
const char ioText_effectorsInit[] PROGMEM = "Effectors initing ";
const char ioText_pilotInit[] PROGMEM = "Pilot initing ";
const char ioText_OK[] PROGMEM = "OK\n\r";
const char ioText_ERROR[] PROGMEM = "ERROR\n\r";
const char ioText_imuInitERROR[] PROGMEM = " IMU/DMP is ded ";
const char ioText_barometerInitERROR[] PROGMEM = " BAROMETER is ded ";
const char ioText_bootTimeIs[] PROGMEM = "Boot time: %d";
const char ioText_lps[] PROGMEM = "LPS: %d";
const char ioText_freeRamIs[] PROGMEM = "FREERAM: %d";


#define IO_SERIAL_STREAM Serial
#define IO_SERIAL_BAUD 115200

#define IO_REP_REG_LEN 23

#define IO_REP_M1 0
#define IO_REP_M2 1
#define IO_REP_M3 2
#define IO_REP_M4 3

#define IO_REP_AX 4
#define IO_REP_AY 5
#define IO_REP_AZ 6

#define IO_REP_GX 7
#define IO_REP_GY 8
#define IO_REP_GZ 9

#define IO_REP_MX 10
#define IO_REP_MY 11
#define IO_REP_MZ 12

#define IO_REP_BARO 13
#define IO_REP_BAROALT 19

#define IO_REP_SONAR 14

#define IO_REP_BAT 15

#define IO_REP_LPS 16

#define IO_REP_GIMX 17
#define IO_REP_GIMY 18

// Realy need? yaw
#define IO_REP_YAW 20 
#define IO_REP_ROLL 21
#define IO_REP_PITCH 22

class ioWrapper
{
	public:
		/* Init all sensors */
		void init();
		/* Send message to operator */
		void sendMesg(const char* msg);
		/* Send message to operator whit some int value */
		void sendMesg(const char* msg,int value);
		/* Start sending message */
		void sendMesgNoNl(const char* msg);
		/* add some data to message */
		void sendMesgNoStart(const char* msg);
		/* flush message */
		/* TODO :) */
		/* Do stuff to read and write */
		void update();
		/* Repport some values, do I need that? */
		void raportValue(uint8_t repKey,uint16_t repVal);
		void triggerCriticalError();

		/* Target values, they need to be moved to pilot */
		float gimbalTargetRoll = 0;
		float gimbalTargetPitch = 0;
		
	private:
		char buffer[IO_MAX_MSG_LEN];
		float repReg[IO_REP_REG_LEN];

		/* Operator command interpreter */
		void runCommand(char* cmd,uint8_t len);
};

void ioWrapper::raportValue(uint8_t repKey,uint16_t repVal){
	repReg[repKey] = repVal;
}

void ioWrapper::init(){
	IO_SERIAL_STREAM.begin(IO_SERIAL_BAUD);
	for(int i = IO_MAX_MSG_LEN-1;i;i--)buffer[i]=0;
	buffer[0]='|';
	for(int i=0;i<IO_REP_REG_LEN;i++)
		repReg[i] = 0;
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
					gimbalTargetRoll  = ((float)(cmd[2]-'1'))/5.f-1;
					break;
				case 'y':
				case 'Y':
					gimbalTargetPitch = ((float)(cmd[2]-'1'))/5.f-1;
					break;
			}
			break;
	}
}
void ioWrapper::update(){
	static char inBuff[16];
	static uint8_t inBuffPos=0;
	static uint16_t uno = 0;
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
	if(++uno == 150){
		uno = 0;
		IO_SERIAL_STREAM.println(repReg[IO_REP_LPS]);
		// IO_SERIAL_STREAM.print(' ');
		// // dtostrf(repReg[IO_REP_ROLL],6,6,b);
		// IO_SERIAL_STREAM.print(b);
		// IO_SERIAL_STREAM.print(' ');
		// // dtostrf(repReg[IO_REP_PITCH],6,6,b);
		// IO_SERIAL_STREAM.print(b);
		// IO_SERIAL_STREAM.print(' ');
		// // dtostrf(repReg[IO_REP_GZ],6,6,b);
		// IO_SERIAL_STREAM.println(b);
		// // for(int i=0;i<IO_REP_REG_LEN;i++){
		// // 	IO_SERIAL_STREAM.print("| regNo");
		// // 	IO_SERIAL_STREAM.print(i);
		// // 	IO_SERIAL_STREAM.print(": ");
		// // 	IO_SERIAL_STREAM.println(repReg[i]);
		// }
	}
}
void ioWrapper::triggerCriticalError(){
	while(1);
}

#endif