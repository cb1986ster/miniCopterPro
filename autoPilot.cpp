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
void autoPilot::saveInEEPROM(uint8_t a, uint8_t v){
	EEPROM.write(a,v);
}
uint8_t autoPilot::getEEPROM(uint8_t a){
	return EEPROM.read(a);
}
void autoPilot::initPID(){
	double pidValues[4][3] =
		{
			{0,0,0},
			{0,0,0},
			{0,0,0},
			{0,0,0}
		};
	if(
		170 == EEPROM.read(3) &&
		170 == EEPROM.read(2) &&
		170 == EEPROM.read(1) &&
		170 == EEPROM.read(0)
	){
		for(uint8_t i=0;i<sizeof(pidValues);i++)
			((byte*)pidValues)[i]=EEPROM.read(i+4);
	}
	// pidRoll = new PID(
	// 	((miniCopterPro*)copterPointer)->sensors.getRollPointer(),
	// 	&pidRollValue,
	// 	((miniCopterPro*)copterPointer)->getRollTargetPointer(),
	// 	pidValues[0][0],pidValues[0][1],pidValues[0][2],
	// 	DIRECT
	// );
	pidPitch = new PID(
		((miniCopterPro*)copterPointer)->sensors.getPitchPointer(),
		&pidPitchValue,
		((miniCopterPro*)copterPointer)->getPitchTargetPointer(),
		pidValues[1][0],pidValues[1][1],pidValues[1][2],
		DIRECT
	);
	// pidYaw = new PID(
	// 	((miniCopterPro*)copterPointer)->sensors.getYawPointer(),
	// 	&pidYawValue,
	// 	((miniCopterPro*)copterPointer)->getYawTargetPointer(),
	// 	pidValues[2][0],pidValues[2][1],pidValues[2][2],
	// 	DIRECT
	// );
	// pidThrotle = new PID(
	// 	((miniCopterPro*)copterPointer)->sensors.getAltChangePointer(),
	// 	&pidYawValue,
	// 	((miniCopterPro*)copterPointer)->getAltChangeTargetPointer(),
	// 	pidValues[3][0],pidValues[3][1],pidValues[3][2],
	// 	DIRECT
	// );
}

void autoPilot::fixPlatform(){
	// pidRoll->Compute();
	pidPitch->Compute();
	// // pidThrotle->Compute();
	// // pidYaw->Compute();

}

void autoPilot::doJob(){
	switch(workMode){
		case PILOT_MODE_DANCE:
			/*********************************************/
			fixPlatform();
			((miniCopterPro*)copterPointer)->effectors.setMotorSpeed(0,
				0.19+pidPitchValue);
			((miniCopterPro*)copterPointer)->effectors.setMotorSpeed(1,
				0.19-pidPitchValue);
			fixGimbal();
			/*###########################################*/
		break;
		case PILOT_MODE_FLY:
			/*********************************************/
			fixPlatform();
			/*###########################################*/
			break;
		case PILOT_MODE_START:
			/*********************************************/
			/*###########################################*/
			break;
		case PILOT_MODE_LANDING:
			/*********************************************/
			/*###########################################*/
		break;
		case PILOT_MODE_EMERGENCY:
			/*********************************************/
			/*###########################################*/
			break;
		case PILOT_MODE_STABLISATION:
			/*********************************************/
			/*###########################################*/
		break;
		case PILOT_MODE_IDLE:
			/*********************************************/
			((miniCopterPro*)copterPointer)->effectors.stopMotors();
			/*###########################################*/
		break;
		case PILOT_MODE_GIMBAL_RUN:
			/*********************************************/
			fixGimbal();
			((miniCopterPro*)copterPointer)->effectors.stopMotors();
			/*###########################################*/
		break;
		case PILOT_MODE_MOTOR_TEST:
			/*********************************************/
			((miniCopterPro*)copterPointer)->effectors.setMotorSpeed(0,
				((miniCopterPro*)copterPointer)->getPitchTarget()
			);
			((miniCopterPro*)copterPointer)->effectors.setMotorSpeed(1,
				((miniCopterPro*)copterPointer)->getRollTarget()
			);
			/*###########################################*/
		break;
	}
}
