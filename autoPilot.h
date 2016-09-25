#ifndef _AUTOPILOT_H_
#define _AUTOPILOT_H_ 1


enum PILOT_MODE
{
	PILOT_MODE_IDLE,
	PILOT_MODE_MOTOR_TEST,
	PILOT_MODE_GIMBAL_RUN,
	PILOT_MODE_START,
	PILOT_MODE_LANDING,
	PILOT_MODE_EMERGENCY,
	PILOT_MODE_STABLISATION,
	PILOT_MODE_FLY,
	PILOT_MODE_DANCE
};

enum CPID
{
	CPID_ROLL,
	CPID_PITCH,
	CPID_ROT,
	CPID_THROTLE,
	CPID_ALL_NO
};

#include "ioWrapper.h"
#include "effectorsWrapper.h"
#include "sensorsWrapper.h"
#include <EEPROM.h>
#include <PID_v1.h>

class autoPilot
{
	public:
		void installIn(void* mcp){copterPointer = mcp;};
		void init();
		void doJob();
		void setMode(PILOT_MODE newMode){workMode = newMode;}; // if(workMode!=PILOT_MODE_EMERGENCY) ??
		void saveInEEPROM(uint8_t a, uint8_t v){EEPROM.write(a,v);};
		uint8_t getEEPROM(uint8_t a){return EEPROM.read(a);};
		void initPID();
		void tunePID(int pidNo,double* pidK);
	private:
		void* copterPointer;
		PILOT_MODE workMode;
		void fixGimbal();
		void fixPlatform();
		double* pidDataPointers[4][2];
		PID *pids[CPID_ALL_NO]={0,0,0,0};
		double pidsOut[CPID_ALL_NO];
};
#endif
