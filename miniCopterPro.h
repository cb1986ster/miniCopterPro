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
		void setup();
		void loop();
		ioWrapper               io; // Communication
		sensorsWrapper     sensors; // Sensors
		effectorsWrapper effectors; // Motors, gimball ect
		autoPilot            pilot; // Brain
		watchDog                wd; // WatchDog

		void setGimbalTarget(uint8_t aixs,float target){gimbalTarget[aixs] = target;};
		float getGimbalTarget(uint8_t aixs){return gimbalTarget[aixs];};

		void setPlatformTarget(uint8_t aixs,float target){patformTarget[aixs] = target;};
		void setRotationTarget(float target){rotationTarget = target;};
		void setAltChangeTarget(float target){altChangeTarget = target;};

		double* getRollTargetPointer(){return (double*)patformTarget;}
		double* getPitchTargetPointer(){return ((double*)patformTarget)+1;}
		double* getYawTargetPointer(){return (double*)&rotationTarget;}
		double* getAltChangeTargetPointer(){return (double*)&altChangeTarget;}

		double getRollTarget(){return *(patformTarget);}
		double getPitchTarget(){return *(patformTarget+1);}
		double getYawTarget(){return rotationTarget;}
		double getAltChangeTarget(){return altChangeTarget;}

	private:
		float gimbalTarget[2];
		float patformTarget[2];
		float rotationTarget;
		float altChangeTarget;
};
#endif
