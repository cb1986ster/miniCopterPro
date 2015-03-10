#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_ 1

#include <freeram.h>

class watchDog
{
	public:
		void connectIo(ioWrapper* newOne);
		void connectSensors(sensorsWrapper* newOne);
		void connectEffectors(effectorsWrapper* newOne);
		void connectPilot(autoPilot* newOne);
		void check();
		watchDog();
		void initDone();
	public:
		ioWrapper* io;
		sensorsWrapper* sensors;
		effectorsWrapper* effectors;
		autoPilot* pilot;
		long unsigned int lastTimer;
		long unsigned int lpsTimer;
		long unsigned int tmNow;
		uint32_t lps;
};

watchDog::watchDog(){
	lastTimer = millis();
}
void watchDog::initDone(){
	tmNow = millis();
	io->sendMesg(ioText_bootTimeIs,tmNow-lastTimer);
	io->sendMesg(ioText_freeRamIs,freeRam());
	lps = 0;
	lpsTimer = tmNow +1000;
}

void watchDog::connectIo(ioWrapper* newOne){
	io = newOne;
}
void watchDog::connectSensors(sensorsWrapper* newOne){
	sensors = newOne;
}
void watchDog::connectEffectors(effectorsWrapper* newOne){
	effectors = newOne;
}
void watchDog::connectPilot(autoPilot* newOne){
	pilot = newOne;
}
void watchDog::check(){
	tmNow = millis();
	/* Report lps */
	if(tmNow > lpsTimer){
		io->raportValue(IO_REP_LPS,lps);
		lpsTimer = tmNow + 1000;	
		lps = 0;
	} 
	lps++;
}

#endif