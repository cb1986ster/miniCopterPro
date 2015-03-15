#ifndef _IOWRAPPER_H_
#define _IOWRAPPER_H_ 1

#define IO_SERIAL_STREAM Serial
#define IO_SERIAL_BAUD 57600

#include <Arduino.h>
#include <avr/pgmspace.h> /* For PROGMEM */

#define IO_MAX_MSG_LEN 19
/* GLOBALS !!! */
const char ioText_ioInited[] PROGMEM = "IO init successful. Starting :D";
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
		/* WatchDog? */
		void triggerCriticalError();
		/* Install in copter */
		void installIn(void* mcp){copterPointer = mcp;};
		/* Send copter status */
		void sendStatus();
		
	private:
		void* copterPointer;

		char buffer[IO_MAX_MSG_LEN];

		/* Operator command interpreter */
		void runCommand(char* cmd,uint8_t len);
};

#endif