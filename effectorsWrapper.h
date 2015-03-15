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

class effectorsWrapper
{
	public:
		void installIn(void* mcp){copterPointer = mcp;};
		void init();
		void update();	
		void setMotorSpeed(uint8_t motorNo, float newSpeed){motorsSpeed[motorNo] = newSpeed;};
		void setGimbalArc(uint8_t motorNo, float newArc){gimbalArc[motorNo] = newArc;};

	private:
		uint8_t i; // tmp var
		void* copterPointer;

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

#endif