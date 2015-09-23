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
	PILOT_MODE_FLY
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
		void saveInEEPROM(uint8_t a, uint8_t v);
		uint8_t getEEPROM(uint8_t a);
	private:
		void initPID();
		void* copterPointer;
		PILOT_MODE workMode = PILOT_MODE_IDLE;
		void fixGimbal();
		void fixPlatform();
		PID* pidRoll;
		double pidRollValue = 0;
		PID* pidPitch;
		double pidPitchValue = 0;
		PID* pidYaw;
		double pidYawValue = 0;
		PID* pidThrotle;
		double pidThrotleValue = 0;
};
#endif