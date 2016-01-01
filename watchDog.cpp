#include "watchDog.h"

#include "miniCopterPro.h"


void watchDog::reset(){
	((miniCopterPro*)copterPointer)->setPlatformTarget(0,0);
	((miniCopterPro*)copterPointer)->setPlatformTarget(1,0);
	((miniCopterPro*)copterPointer)->setRotationTarget(0);
	((miniCopterPro*)copterPointer)->setAltChangeTarget(0);
	((miniCopterPro*)copterPointer)->setGimbalTarget(0 , 1);
	((miniCopterPro*)copterPointer)->setGimbalTarget(1 , 0);
}


watchDog::watchDog(){
	lastTimer = millis();
}
void watchDog::initDone(){
	tmNow = millis();
#ifdef _WATCHDOG_FEATURE_FREERAM
	((miniCopterPro*)copterPointer)->io.sendMesg(ioText_freeRamIs,freeRam());
#endif
	lps = 0;
	lpsTimer = tmNow +100;
#ifdef _WATCHDOG_FEATURE_BATTERY_LOW_AUTO_LANDING
	lastBatteryState = ((miniCopterPro*)copterPointer)->sensors.getBatteryStatus();
#endif
}

void watchDog::check(){
#ifdef _WATCHDOG_FEATURE_BATTERY_LOW_AUTO_LANDING
	static double batteryState = 0;
	static double batteryTimeLeft = 0;
	static double minimalLandingTime = 0;
#endif
	tmNow = millis();
	/* Report lps */
	if(tmNow > lpsTimer){
		((miniCopterPro*)copterPointer)->io.sendStatus();
		lpsTimer = tmNow + 100;
		lps = 0;
#ifdef _WATCHDOG_FEATURE_BATTERY_LOW_AUTO_LANDING
		// lowpass battery voltage value
		batteryState = ((miniCopterPro*)copterPointer)->sensors.getBatteryStatus()*0.3f - lastBatteryState*0.7f;
		// battery live time left in sec
		batteryTimeLeft = batteryState*0.1/(lastBatteryState-batteryState);
		// s`` = 4 ; s` = 1.3 ; l = 10
		minimalLandingTime = ((((miniCopterPro*)copterPointer)->sensors.getHeight()-10.0f)/3.5f)+10.0f;
		// If battery live time too low
		if( minimalLandingTime > batteryTimeLeft){
			((miniCopterPro*)copterPointer)->pilot.setMode(PILOT_MODE_EMERGENCY);
		}
#endif
	}
	lps++;
}
