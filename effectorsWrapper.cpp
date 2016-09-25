/**
	This file is for my 4 motor, 2-axis gimbal copter
*/
#include "effectorsWrapper.h"

#include "miniCopterPro.h"

void effectorsWrapper::stopMotors(){
	motorsSpeed[0] = 0;
	motorsSpeed[1] = 0;
	motorsSpeed[2] = 0;
	motorsSpeed[3] = 0;
};


void effectorsWrapper::init(){
	((miniCopterPro*)copterPointer)->io.sendMesgNoNl(ioText_effectorsInit);

	gimbalInit();
	motorInit();
#ifdef USE_SOFTWARE_SERVO_CONTROL
	SoftwareServo::refresh();
#endif

	/* All done */
	((miniCopterPro*)copterPointer)->io.sendMesgNoStart(ioText_OK);
}

void effectorsWrapper::update(){
	gimbalUpdate();
	motorsUpdate();
#ifdef USE_SOFTWARE_SERVO_CONTROL
	SoftwareServo::refresh();
#endif
}

void effectorsWrapper::gimbalInit(){
#ifdef USE_SOFTWARE_SERVO_CONTROL
	gimbal[0].attach(10);
	gimbal[1].attach(11);
#else
	gimbal[0].attach(10,670,1830);
	gimbal[1].attach(11,670,1830);
#endif
	setGimbalArc(0,0);
	setGimbalArc(1,0);
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
#ifdef USE_SOFTWARE_SERVO_CONTROL
	motor[0].attach(9);
	motor[1].attach(6);
	motor[2].attach(5);
	motor[3].attach(3);
#else
	motor[0].attach(9,700,2000);
	motor[1].attach(6,700,2000);
	motor[2].attach(5,700,2000);
	motor[3].attach(3,700,2000);
#endif
	setMotorSpeed(0,0);
	setMotorSpeed(1,0);
	setMotorSpeed(2,0);
	setMotorSpeed(3,0);
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
