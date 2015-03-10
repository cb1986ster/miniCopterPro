/*
	TODO: usunąć bibliotekę servo.h i przerobić na PWM atmegowe
			http://arduino.cc/en/Tutorial/SecretsOfArduinoPWM
*/
#ifndef _EFFECTORSWRAPPER_H_
#define _EFFECTORSWRAPPER_H_ 1
#include "ioWrapper.h"

// #define ARDUINO_ARCH_AVR TRUE
// #include <Servo.h>
// #include <avr/Servo.cpp>
#include <Servo.h>

#define EFFECTORS_MOTOR_NO 4
#define EFFECTORS_GIMBAL_NO 4

class effectorsWrapper
{
	public:
		void connectIo(ioWrapper* nio);
		void init();
		void update();	
		void setMotorSpeed(uint8_t motorNo, float newSpeed);
		void setGimbalArc(uint8_t motorNo, float newArc);

	private:
		uint8_t i; // tmp var
		ioWrapper* io;

		/* Motors */
		float motorsSpeed[EFFECTORS_MOTOR_NO];
		Servo motor[EFFECTORS_MOTOR_NO];
		void motorInit();
		void motorsUpdate();

		/* Gimbal */
		float gimbalArc[EFFECTORS_GIMBAL_NO];
		Servo gimbal[EFFECTORS_GIMBAL_NO];	
		void gimbalInit();
		void gimbalUpdate();
};

void effectorsWrapper::init(){
	io->sendMesgNoNl(ioText_effectorsInit);

	gimbalInit();
	motorInit();

	/* All done */
	io->sendMesgNoStart(ioText_OK);
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

void effectorsWrapper::setMotorSpeed(uint8_t motorNo, float newSpeed){
	motorsSpeed[motorNo] = newSpeed;
}

void effectorsWrapper::motorsUpdate(){
	i = constrain((int)(motorsSpeed[0]*180.0),5,180);
	motor[0].write(i); io->raportValue(IO_REP_M1,i);
	i = constrain((int)(motorsSpeed[1]*180.0),5,180);
	motor[1].write(i); io->raportValue(IO_REP_M2,i);
	i = constrain((int)(motorsSpeed[2]*180.0),5,180);
	motor[2].write(i); io->raportValue(IO_REP_M3,i);
	i = constrain((int)(motorsSpeed[3]*180.0),5,180);
	motor[3].write(i); io->raportValue(IO_REP_M4,i);
}

void effectorsWrapper::connectIo(ioWrapper* nio){
	io = nio;
}

void effectorsWrapper::update(){
	motorsUpdate();
	gimbalUpdate();
}

void effectorsWrapper::gimbalInit(){
	gimbal[0].attach(10,670,1830);
	gimbal[1].attach(11,670,1830);
	for(i=0; i<EFFECTORS_GIMBAL_NO ;i++)
		setGimbalArc(i,0);
	gimbalUpdate();
}

void effectorsWrapper::gimbalUpdate(){
	i = constrain((int)(gimbalArc[0]*71)+71,3,142);
	gimbal[0].write(i); io->raportValue(IO_REP_GIMX,i);
	i = constrain((int)(gimbalArc[1]*71)+71,3,142);
	gimbal[1].write(i); io->raportValue(IO_REP_GIMY,i);
}


void effectorsWrapper::setGimbalArc(uint8_t motorNo, float newArc){
	gimbalArc[motorNo] = newArc;
}

#endif