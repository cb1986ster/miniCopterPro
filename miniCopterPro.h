#ifndef _MINICOPTERPRO_H_
#define _MINICOPTERPRO_H_ 1

#include "ioWrapper.h"
#include "effectorsWrapper.h"
#include "sensorsWrapper.h"
#include "autoPilot.h"
#include "watchDog.h"

class miniCopterPro
{
	public:
		void fly();	
		ioWrapper io;
		sensorsWrapper sensors;
		effectorsWrapper effectors;
		autoPilot pilot;
		watchDog wd;
		void setGimbalTarget(uint8_t aixs,float target){gimbalTarget[aixs] = target;};
		float getGimbalTarget(uint8_t aixs){return gimbalTarget[aixs];};

		void setPlatformTarget(uint8_t aixs,float target){patformTarget[aixs] = target;};
		void setRotationTarget(float target){rotationTarget = target;};
		void setAltChangeTarget(float target){altChangeTarget = target;};
	private:
		void setup();
		void loop();
		float gimbalTarget[2];
		float patformTarget[2];
		float rotationTarget;
		float altChangeTarget;
};

#endif