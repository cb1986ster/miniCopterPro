#include "effectorsWrapper.h"

#include "miniCopterPro.h"

void effectorsWrapper::init(){
	((miniCopterPro*)copterPointer)->io.sendMesgNoNl(ioText_effectorsInit);

	gimbalInit();
	motorInit();

	/* All done */
	((miniCopterPro*)copterPointer)->io.sendMesgNoStart(ioText_OK);
}

void effectorsWrapper::update(){
	gimbalUpdate();
	motorsUpdate();
}

void effectorsWrapper::gimbalInit(){
	gimbal[0].attach(10,670,1830);
	gimbal[1].attach(11,670,1830);
	for(i=0; i<EFFECTORS_GIMBAL_NO ;i++)
		setGimbalArc(i,0);
	gimbalUpdate();
}

void effectorsWrapper::gimbalUpdate(){
	gimbal[0].write(
		constrain((int)(gimbalArc[0]*71)+71,3,142)
	);
	gimbal[1].write(
		constrain((int)(gimbalArc[1]*71)+71,3,142)
	);
}

void effectorsWrapper::motorInit(){
	motor[0].attach(9,700,2000);
	motor[1].attach(6,700,2000);
	motor[2].attach(5,700,2000);
	motor[3].attach(3,700,2000);
	for(i=0; i<EFFECTORS_MOTOR_NO ;i++)
		setMotorSpeed(i,0);
	motorsUpdate();
}

void effectorsWrapper::motorsUpdate(){
	motor[0].write(
		constrain((int)(motorsSpeed[0]*180.0),EFFECTORS_MOTOR_MIN_VALUE,EFFECTORS_MOTOR_MAX_VALUE)
	); 
	motor[1].write(
		constrain((int)(motorsSpeed[1]*180.0),EFFECTORS_MOTOR_MIN_VALUE,EFFECTORS_MOTOR_MAX_VALUE)
	); 
	motor[2].write(
		constrain((int)(motorsSpeed[2]*180.0),EFFECTORS_MOTOR_MIN_VALUE,EFFECTORS_MOTOR_MAX_VALUE)
	); 
	motor[3].write(
		constrain((int)(motorsSpeed[3]*180.0),EFFECTORS_MOTOR_MIN_VALUE,EFFECTORS_MOTOR_MAX_VALUE)
	); 
}