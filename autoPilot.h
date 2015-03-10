#ifndef _AUTOPILOT_H_
#define _AUTOPILOT_H_ 1

#define PILOTMODE_IDLE 0
#define PILOTMODE_MOTOR_TEST 1

#include "ioWrapper.h"
#include "effectorsWrapper.h"
#include "sensorsWrapper.h"

class autoPilot
{
	public:
		void connectIo(ioWrapper* newOne);
		void connectSensors(sensorsWrapper* newOne);
		void connectEffectors(effectorsWrapper* newOne);
		void init();
		void doJob();
		void setMode(uint8_t newMode);
	private:
		ioWrapper* io;
		sensorsWrapper* sensors;
		effectorsWrapper* effectors;
		uint8_t workMode;
		void fixGimbalPosition();
};

void autoPilot::connectIo(ioWrapper* newOne){
	io = newOne;
}
void autoPilot::connectSensors(sensorsWrapper* newOne){
	sensors = newOne;
}
void autoPilot::connectEffectors(effectorsWrapper* newOne){
	effectors = newOne;
}
void autoPilot::init(){
	io->sendMesgNoNl(ioText_pilotInit);
	workMode = 0;	
	io->sendMesgNoStart(ioText_OK);
}
void autoPilot::fixGimbalPosition(){
	effectors->setGimbalArc(0,io->gimbalTargetRoll +(sensors->roll/90.f));
	effectors->setGimbalArc(1,io->gimbalTargetPitch-(sensors->pitch/90.f));
}
void autoPilot::doJob(){
	static uint8_t mno = 0;
	static float cspeed = 0;
	fixGimbalPosition();
	switch(workMode){
		case PILOTMODE_IDLE:
			effectors->setMotorSpeed(0,0);
			effectors->setMotorSpeed(1,0);
			effectors->setMotorSpeed(2,0);
			effectors->setMotorSpeed(3,0);
		break;
		case PILOTMODE_MOTOR_TEST:
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
			effectors->setMotorSpeed(mno/2,cspeed);
			delay(10);
			break;
	}
}
 
void autoPilot::setMode(uint8_t newMode){
	workMode = newMode;
}

#endif