#include "watchDog.h"

#include "miniCopterPro.h"

watchDog::watchDog(){
	lastTimer = millis();
}
void watchDog::initDone(){
	tmNow = millis();
	((miniCopterPro*)copterPointer)->io.sendMesg(ioText_bootTimeIs,tmNow-lastTimer);
	((miniCopterPro*)copterPointer)->io.sendMesg(ioText_freeRamIs,freeRam());
	lps = 0;
	lpsTimer = tmNow +1000;
}

void watchDog::check(){
	tmNow = millis();
	/* Report lps */
	if(tmNow > lpsTimer){
		((miniCopterPro*)copterPointer)->io.sendStatus();
		lpsTimer = tmNow + 1000;	
		lps = 0;
	} 
	lps++;
}