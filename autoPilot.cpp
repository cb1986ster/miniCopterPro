#include "autoPilot.h"

#include "miniCopterPro.h"

void autoPilot::init(){
	((miniCopterPro*)copterPointer)->io.sendMesgNoNl(ioText_pilotInit);
	workMode = PILOT_MODE_GIMBAL_RUN;	
	initPID();
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

void autoPilot::initPID(){
	pidRoll = new PID(
		((miniCopterPro*)copterPointer)->sensors.getRollPointer(),
		&pidRollValue,
		((miniCopterPro*)copterPointer)->getRollTargetPointer(),
		0.0,0.0,0.0,
		DIRECT
	);
	pidPitch = new PID(
		((miniCopterPro*)copterPointer)->sensors.getPitchPointer(),
		&pidPitchValue,
		((miniCopterPro*)copterPointer)->getPitchTargetPointer(),
		0.0,0.0,0.0,
		DIRECT
	);
	pidYaw = new PID(
		((miniCopterPro*)copterPointer)->sensors.getYawPointer(),
		&pidYawValue,
		((miniCopterPro*)copterPointer)->getYawTargetPointer(),
		0.0,0.0,0.0,
		DIRECT
	);
	pidThrotle = new PID(
		((miniCopterPro*)copterPointer)->sensors.getAltChangePointer(),
		&pidYawValue,
		((miniCopterPro*)copterPointer)->getAltChangeTargetPointer(),
		0.0,0.0,0.0,
		DIRECT
	);
}

void autoPilot::fixPlatform(){
	// TODO:this
}

void autoPilot::doJob(){
	static uint8_t mno = 0;
	static float cspeed = 0;
	switch(workMode){
		case PILOT_MODE_FLY:
			break;
		case PILOT_MODE_START:
			break;
		case PILOT_MODE_LANDING:
			break;
		case PILOT_MODE_EMERGENCY:
			break;
		case PILOT_MODE_STABLISATION:
			break;
		case PILOT_MODE_IDLE:
			((miniCopterPro*)copterPointer)->effectors.setMotorSpeed(0,0);
			((miniCopterPro*)copterPointer)->effectors.setMotorSpeed(1,0);
			((miniCopterPro*)copterPointer)->effectors.setMotorSpeed(2,0);
			((miniCopterPro*)copterPointer)->effectors.setMotorSpeed(3,0);
		break;
		case PILOT_MODE_GIMBAL_RUN:
			fixGimbal();
			((miniCopterPro*)copterPointer)->effectors.setMotorSpeed(0,0);
			((miniCopterPro*)copterPointer)->effectors.setMotorSpeed(1,0);
			((miniCopterPro*)copterPointer)->effectors.setMotorSpeed(2,0);
			((miniCopterPro*)copterPointer)->effectors.setMotorSpeed(3,0);
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