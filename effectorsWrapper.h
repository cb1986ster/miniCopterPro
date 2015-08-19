/*
	TODO: usunąć bibliotekę servo.h i przerobić na PWM atmegowe
			http://arduino.cc/en/Tutorial/SecretsOfArduinoPWM
*/
#ifndef _EFFECTORSWRAPPER_H_
#define _EFFECTORSWRAPPER_H_ 1

#include <Arduino.h>
#include <Servo.h>

#include "ioWrapper.h"

#define EFFECTORS_MOTOR_NO 4
#define EFFECTORS_GIMBAL_NO 4

#define EFFECTORS_MOTOR_MAX_VALUE 96
#define EFFECTORS_MOTOR_MIN_VALUE 3

class effectorsWrapper
{
	public:
		void installIn(void* mcp){copterPointer = mcp;};
		void init();
		void update();	
		void setMotorSpeed(uint8_t motorNo, float newSpeed){motorsSpeed[motorNo] = newSpeed;};
		void setGimbalArc(uint8_t motorNo, float newArc){gimbalArc[motorNo] = newArc;};
		float getMotorSpeed(uint8_t motorNo){return constrain((int)(motorsSpeed[motorNo]*180.0),EFFECTORS_MOTOR_MIN_VALUE,EFFECTORS_MOTOR_MAX_VALUE);}

	private:
		uint8_t i; // tmp var
		void* copterPointer;

		/* Motors */
		float motorsSpeed[EFFECTORS_MOTOR_NO];
		Servo motor[EFFECTORS_MOTOR_NO];
		inline void motorInit();
		void motorsUpdate();

		/* Gimbal */
		float gimbalArc[EFFECTORS_GIMBAL_NO];
		Servo gimbal[EFFECTORS_GIMBAL_NO];	
		inline void gimbalInit();
		void gimbalUpdate();
};

#endif