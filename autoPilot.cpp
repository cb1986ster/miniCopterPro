#include "autoPilot.h"

#include "miniCopterPro.h"

void autoPilot::init(){
	((miniCopterPro*)copterPointer)->io.sendMesgNoNl(ioText_pilotInit);
	workMode = PILOT_MODE_IDLE;	
	((miniCopterPro*)copterPointer)->io.sendMesgNoStart(ioText_OK);
}
void autoPilot::fixGimbal(){
	((miniCopterPro*)copterPointer)->effectors.setGimbalArc(
		0,((miniCopterPro*)copterPointer)->getGimbalTarget(0)+2*((miniCopterPro*)copterPointer)->sensors.getRoll()
	);
	((miniCopterPro*)copterPointer)->effectors.setGimbalArc(
		1,((miniCopterPro*)copterPointer)->getGimbalTarget(1)-2*((miniCopterPro*)copterPointer)->sensors.getPitch()
	);
}

void autoPilot::doJob(){
	static uint8_t mno = 0;
	static float cspeed = 0;
	switch(workMode){
		case PILOT_MODE_IDLE:
		break;
		case PILOT_MODE_GIMBAL_RUN:
			fixGimbal();
		break;
		case PILOT_MODE_MOTOR_TEST:
			if(mno % 2 ==0) cspeed += 0.001;
			else cspeed -= 0.001;
			if(cspeed > 0.2){
				cspeed = 0.2;
				mno++;
			} else if(cspeed < 0) {
				cspeed = 0;
				if(mno==7)mno=0;
				else mno++;
			}
			((miniCopterPro*)copterPointer)->effectors.setMotorSpeed(mno/2,cspeed);
			delay(10);
			break;
	}
}