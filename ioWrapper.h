#ifndef _IOWRAPPER_H_
#define _IOWRAPPER_H_ 1

#define IO_SERIAL_STREAM Serial
#define IO_SERIAL_BAUD 57600

#include <Arduino.h>
#include <avr/pgmspace.h> /* For PROGMEM */

#define IO_MAX_MSG_LEN 24
#define DECIMAL_PRECISION 100

/* GLOBALS !!! */
const char ioText_ioInited[] PROGMEM = "IO OK";
const char ioText_sensorsInit[] PROGMEM = "SENS";
const char ioText_effectorsInit[] PROGMEM = "EFE";
const char ioText_pilotInit[] PROGMEM = "PILOT";
const char ioText_OK[] PROGMEM = " OK\n\r";
const char ioText_ERROR[] PROGMEM = " ERROR\n\r";
// const char ioText_imuInitERROR[] PROGMEM = " IMU/DMP is ded ";
const char ioText_barometerInitERROR[] PROGMEM = " BAROM ERR";
const char ioText_freeRamIs[] PROGMEM = "FR: %d";

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
		void writeValue(const float val){
			static uint16_t z,f;
			z = val;
			f = (val*DECIMAL_PRECISION) - z*DECIMAL_PRECISION;
			IO_SERIAL_STREAM.write(z);
			IO_SERIAL_STREAM.write('.');
			IO_SERIAL_STREAM.write(f);
		};
};

#endif