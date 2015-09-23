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
		float getRotation(){return zRotation;};
		double getHeight(){return (sonarAlt < 5.0 && sonarAlt > 0.0)?sonarAlt:baroAlt;};
		double getBatteryStatus(){return batteryStatus;};
		double* getRollPointer(){return (double*)&roll;}
		double* getPitchPointer(){return (double*)&pitch;}
		double* getYawPointer(){return (double*)&zRotation;}
		double* getAltChangePointer(){return (double*)&altChange;}

	private:
		float roll;
		float pitch;
		float zRotation;

		double altChange;

		uint8_t tmp;
		void* copterPointer;
		/* IMU */
		inline void imuInit();
		inline void imuUpdate();

		/* Battery status */
		double batteryStatus;
		inline void batteryInit();
		inline void batteryUpdate();

		/* Sonar */
		NewPing* sonar;
		uint16_t sonarAlt;
		inline void sonarInit();
		inline void sonarUpdate();

		/* Barometer */
		SFE_BMP180 pressure;
		double baselinePressureValue;
		double pressureValue;
		double baselineTemperatureValue;
		double temperatureValue;
		double baroAlt;
		double getPressure();
		inline void baroInit();
		inline void baroUpdate();
};

#endif