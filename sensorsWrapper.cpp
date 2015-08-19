#include "sensorsWrapper.h"

#include "miniCopterPro.h"

void sensorsWrapper::baroUpdate(){
	static char mode = 0;
	static char status = 0;
	static unsigned long dtime = 0;
	switch (mode) {
		case 0: // Rozpoczynanie pomiaru temperatury
				status = pressure.startTemperature();
				if (status != 0){
					mode = 1;
					dtime = millis() + status;
				} // TODO: WatchDog!!!
			break;
		case 1: // Oczekiwanie na zakończenie pomiaru temperatury
				if(millis() > dtime){// Ok - temperatutra zmierzona 
					status = pressure.getTemperature(temperatureValue);
					if (status == 0){
						// TODO: callWatchDog!!!
						mode = 0;
					} else {
						status = pressure.startPressure(5);
						if (status == 0){
							// TODO: callWatchDog!!
							mode = 0;
						} else {
							dtime = millis() + status;
							mode = 2;
						}
					}
				}
			break;
		case 2: // Oczekiwanie na zakończenie pomiaru ciśnienia
				if(millis() > dtime){// Ok - Ciśnienie zmierzone
					if (pressure.getPressure(pressureValue,temperatureValue)){
						baroAlt = pressure.altitude(pressureValue,baselinePressureValue);
					}
					// else callWatchDog!!!	
					mode = 0; 
				}
			break;
	}
};
void sensorsWrapper::baroInit(){
	if (pressure.begin()){
		baselinePressureValue = getPressure();	
		baroUpdate();
	} else {
		((miniCopterPro*)copterPointer)->io.sendMesgNoStart(ioText_barometerInitERROR);
		((miniCopterPro*)copterPointer)->io.triggerCriticalError();
	}	
};

double sensorsWrapper::getPressure(){
	char status;
	status = pressure.startTemperature();
	if (status != 0){
		delay(status);
		status = pressure.getTemperature(baselineTemperatureValue);
		if (status != 0){
			temperatureValue = baselineTemperatureValue;
			status = pressure.startPressure(10);
			if (status != 0){
				delay(status);
				status = pressure.getPressure(baselinePressureValue,baselineTemperatureValue);
				if (status != 0)
					return baselinePressureValue;
				else { ((miniCopterPro*)copterPointer)->io.sendMesgNoStart(ioText_barometerInitERROR); ((miniCopterPro*)copterPointer)->io.triggerCriticalError(); }
			}
			else { ((miniCopterPro*)copterPointer)->io.sendMesgNoStart(ioText_barometerInitERROR); ((miniCopterPro*)copterPointer)->io.triggerCriticalError(); }
		}
		else { ((miniCopterPro*)copterPointer)->io.sendMesgNoStart(ioText_barometerInitERROR); ((miniCopterPro*)copterPointer)->io.triggerCriticalError(); }
	}
	else { ((miniCopterPro*)copterPointer)->io.sendMesgNoStart(ioText_barometerInitERROR); ((miniCopterPro*)copterPointer)->io.triggerCriticalError(); }
	return -1;
};

void sensorsWrapper::sonarInit(){
	sonar = new NewPing(
		SENSORS_SONAR_TRIGGER_PIN,
		SENSORS_SONAR_ECHO_PIN,
		SENSORS_SONAR_MAX_DISTANCE
	);
};
void sensorsWrapper::sonarUpdate(){
	sonarAlt = sonar->ping_cm();
};

void sensorsWrapper::batteryInit(){
	pinMode(SENSOR_BATT_ANALOGPIN,OUTPUT);
}
void sensorsWrapper::batteryUpdate(){
	batteryStatus = map(analogRead(SENSOR_BATT_ANALOGPIN), 666, 862, 0, 1000)/1000.0f;
}

void sensorsWrapper::imuInit(){
	mympu_open(0,200,172);//136
}


void sensorsWrapper::imuUpdate(){
	if(mympu_update() == 0){/* TODO: ograniczyć liczbę obliczeń funkcji mympu_update() */
		zRotation = mympu.gyro[0];
		roll  = mympu.ypr[2]+180;
		if(roll>180)roll -= 360;
		roll /= 180.f;
		pitch = mympu.ypr[1]/180.f;
	}
}

void sensorsWrapper::calibration(){
	mympu_update();
}

void sensorsWrapper::init(){
	((miniCopterPro*)copterPointer)->io.sendMesgNoNl(ioText_sensorsInit);
	batteryInit();
	sonarInit();
	Fastwire::setup(800,0); // :)
	imuInit();
	baroInit();
	((miniCopterPro*)copterPointer)->io.sendMesgNoStart(ioText_OK);
}
void sensorsWrapper::update(){
	static uint8_t updateNo = 0;
	if(updateNo%100==0)batteryUpdate();
	if(updateNo%21 ==0)sonarUpdate();
	baroUpdate();
	imuUpdate();
	updateNo++;
}