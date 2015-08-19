#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_ 1

// #define _WATCHDOG_FEATURE_BATTERY_LOW_AUTO_LANDING 1 
// #define _WATCHDOG_FEATURE_FREERAM 1 

#include <Arduino.h>
#ifdef _WATCHDOG_FEATURE_FREERAM 
#include <freeram.h>
#endif

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
#ifdef _WATCHDOG_FEATURE_BATTERY_LOW_AUTO_LANDING		
		float lastBatteryState;
#endif
		uint16_t lps;
};

#endif