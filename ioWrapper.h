#ifndef _IOWRAPPER_H_
#define _IOWRAPPER_H_ 1

#define IO_SERIAL_STREAM Serial
#define IO_SERIAL_BAUD 57600

#include <Arduino.h>
#include <stdlib.h>

#define IO_MAX_MSG_LEN 24

#define SERIAL_WRITE_NATIVE_FLOAT 0
#define SERIAL_WRITE_FLOAT_IMP_NO 1

/* GLOBALS !!! */
const char ioText_ioInited[] = "IO OK";
const char ioText_sensorsInit[] = "SENS";
const char ioText_effectorsInit[] = "EFE";
const char ioText_pilotInit[] = "PILOT";
const char ioText_OK[] = " OK\n\r";
const char ioText_ERROR[] = " ERROR\n\r";
// const char ioText_imuInitERROR[] = " IMU/DMP is ded ";
const char ioText_barometerInitERROR[] = " BAROM ERR";
const char ioText_freeRamIs[] = "FR: %d";

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

		/* Operator command interpreter */
		inline void runCommand(char* cmd,uint8_t len);

		void sendSeparator() {IO_SERIAL_STREAM.write(',');};
		void lineBegin() {IO_SERIAL_STREAM.write('|');};
		void lineEnd() {IO_SERIAL_STREAM.write('\n');};
		void writeValue(float val);
};

#endif
