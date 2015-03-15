#ifndef _AUTOPILOT_H_
#define _AUTOPILOT_H_ 1


enum PILOT_MODE
{
	PILOT_MODE_IDLE,
	PILOT_MODE_MOTOR_TEST,
	PILOT_MODE_GIMBAL_RUN
};

#include "ioWrapper.h"
#include "effectorsWrapper.h"
#include "sensorsWrapper.h"

class autoPilot
{
	public:
		void installIn(void* mcp){copterPointer = mcp;};
		void init();
		void doJob();
		void setMode(PILOT_MODE newMode){workMode = newMode;};
	private:
		void* copterPointer;
		PILOT_MODE workMode = PILOT_MODE_IDLE;
		void fixGimbal();
};

#endif