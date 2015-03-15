#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_ 1

#include <Arduino.h>
#include <freeram.h>

#include "ioWrapper.h"
#include "autoPilot.h"
#include "effectorsWrapper.h"
#include "sensorsWrapper.h"

class watchDog
{
	public:
		watchDog();
		void check();
		void initDone();
		void installIn(void* mcp){copterPointer = mcp;};
		uint16_t getLPS(){return lps;}
	public:
		void* copterPointer;
		long unsigned int lastTimer;
		long unsigned int lpsTimer;
		long unsigned int tmNow;
		uint16_t lps;
};

#endif