#ifndef _SENSORSWRAPPER_H_
#define _SENSORSWRAPPER_H_ 1

#define SENSOR_BATT_ANALOGPIN A0

#define SENSORS_SONAR_TRIGGER_PIN 4 
#define SENSORS_SONAR_ECHO_PIN 2
#define SENSORS_SONAR_MAX_DISTANCE 350

#include <I2Cdev.h>
#include <NewPing.h>
#include <SFE_BMP180.h>
#include <mpu.h>

class sensorsWrapper
{
	public:
		void installIn(void* mcp){copterPointer = mcp;};
		void init();
		void update();
		void calibration();
		float getPitch(){return pitch;};
		float getRoll(){return roll;};
	private:
		float roll;
		float pitch;
		float zRotation;

		uint8_t tmp;
		void* copterPointer;
		/* IMU */
		void imuInit();
		void imuUpdate();

		/* Battery status */
		uint16_t batteryStatus;
		void batteryInit();
		void batteryUpdate();

		/* Sonar */
		NewPing* sonar;
		uint16_t sonarAlt;
		void sonarInit();
		void sonarUpdate();

		/* Barometer */
		SFE_BMP180 pressure;
		double baselinePressureValue;
		double pressureValue;
		double baselineTemperatureValue;
		double temperatureValue;
		double baroAlt;
		double getPressure();
		void baroInit();
		void baroUpdate();
};

#endif