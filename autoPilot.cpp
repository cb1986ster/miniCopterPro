#include "autoPilot.h"

#include "miniCopterPro.h"
#define copter ((miniCopterPro*)copterPointer)
//------------------------------------------------------------------------------
void autoPilot::init(){
	copter->io.sendMesgNoNl(ioText_pilotInit);
	pidDataPointers[0][0]=copter->sensors.getPitchPointer();
	pidDataPointers[0][1]=copter->getPitchTargetPointer();
	pidDataPointers[1][0]=copter->sensors.getRollPointer();
	pidDataPointers[1][1]=copter->getRollTargetPointer();
	pidDataPointers[2][0]=copter->sensors.getYawPointer();
	pidDataPointers[2][1]=copter->getYawTargetPointer();
	pidDataPointers[3][0]=copter->sensors.getPitchPointer();
	pidDataPointers[3][1]=copter->getPitchTargetPointer();
	workMode = PILOT_MODE_GIMBAL_RUN;
	initPID();
	copter->io.sendMesgNoStart(ioText_OK);
}
//------------------------------------------------------------------------------
void autoPilot::fixGimbal(){
	copter->effectors.setGimbalArc(0,copter->sensors.getRoll()+copter->getGimbalTarget(0));
	copter->effectors.setGimbalArc(1,copter->sensors.getPitch()+copter->getGimbalTarget(1));
}
//------------------------------------------------------------------------------
void autoPilot::tunePID(int pidNo,double* pidK){
	uint8_t i;
	for(i=0;i<12;i++)
		saveInEEPROM(i+(pidNo*12), *((byte*)(&pidK)+i) );
	initPID();
}
void autoPilot::initPID(){
	double pidK[3] = {1.0,0.5,0.1};
	uint8_t pidNo,i;
	for(pidNo=0;pidNo<4;pidNo++){
		// Free on reload
		if(pids[pidNo])
			free(pids[pidNo]);
		// Load from EEPROM
		if(
			170 == getEEPROM(3) &&
			170 == getEEPROM(2) &&
			170 == getEEPROM(1) &&
			170 == getEEPROM(0)
		)
			for(i=0;i<12;i++)
				*(((byte*)pidK)+i) = getEEPROM(4 + i + (pidNo*12));
		// Create PID
		pids[pidNo] =
			new PID(
				// Input
				pidDataPointers[pidNo][0],
				// Output
				pidsOut+pidNo,
				// Target
				pidDataPointers[pidNo][2],
				pidK[0], // P
				pidK[1], // I
				pidK[2], // D
				DIRECT
			);
	}
}
//------------------------------------------------------------------------------
void autoPilot::fixPlatform(){
	static double throtle = 0 ;
	pids[CPID_ROLL]->Compute();
	pids[CPID_ROT]->Compute();
	pids[CPID_PITCH]->Compute();
	// pids[CPID_THROTLE]->Compute();
	throtle = copter->getAltChangeTarget();

	copter->effectors.setMotorSpeed(0,
		pidsOut[CPID_ROT] + pidsOut[CPID_ROLL] + pidsOut[CPID_PITCH] + throtle
	);
	copter->effectors.setMotorSpeed(1,
		-pidsOut[CPID_ROT] + pidsOut[CPID_ROLL] - pidsOut[CPID_PITCH] + throtle
	);
	copter->effectors.setMotorSpeed(2,
		pidsOut[CPID_ROT] - pidsOut[CPID_ROLL] - pidsOut[CPID_PITCH] + throtle
	);
	copter->effectors.setMotorSpeed(3,
		-pidsOut[CPID_ROT] - pidsOut[CPID_ROLL] + pidsOut[CPID_PITCH] + throtle
	);
}
//------------------------------------------------------------------------------
void autoPilot::doJob(){
	switch(workMode){
		case PILOT_MODE_FLY:
			/*********************************************/
			fixPlatform();
			fixGimbal();
			/*###########################################*/
			break;
		case PILOT_MODE_IDLE:
			/*********************************************/
			copter->effectors.stopMotors();
			copter->effectors.setGimbalArc(0,0);
			copter->effectors.setGimbalArc(0,1);
			/*###########################################*/
		break;
		case PILOT_MODE_GIMBAL_RUN:
			/*********************************************/
			fixGimbal();
			copter->effectors.stopMotors();
			/*###########################################*/
		break;
		case PILOT_MODE_MOTOR_TEST:
			/*********************************************/
			copter->effectors.setMotorSpeed(0,
				copter->getPitchTarget()
			);
			copter->effectors.setMotorSpeed(1,
				copter->getPitchTarget()
			);
			copter->effectors.setMotorSpeed(2,
				copter->getPitchTarget()
			);
			copter->effectors.setMotorSpeed(3,
				copter->getPitchTarget()
			);
			/*###########################################*/
		break;
	}
}
