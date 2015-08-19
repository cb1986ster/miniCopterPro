#include "miniCopterPro.h"

void miniCopterPro::fly(){
	setup(); /* Init all stuff */
	while(1)loop(); /* Do main loop forever */
}
void miniCopterPro::setup(){
	unsigned long int initCalibrationTime = millis() + 15000;

	/* init IO to user */
	io.installIn(this);
	io.init();

	/* connect io and run */
	effectors.installIn(this);
	effectors.init();
	
	/* connect io and run */
	sensors.installIn(this);
	sensors.init();

	/* connect io,sensors,effectors and run */
	byte value = EEPROM.read(0);
	EEPROM.write(0, value);
	pilot.installIn(this);
	pilot.init();

	/* WATCHDOG. Bark! Bark! */
	wd.installIn(this);
	wd.initDone();

	/* Give time to calibrate all */
	while(millis()<initCalibrationTime){
		sensors.calibration();
	}
}
void miniCopterPro::loop(){
	/* read/send data from/to user */
	io.update();

	/* update sensor readings */
	sensors.update();

	/* Allow pilot to do his job */
	pilot.doJob();

	/* Now commit all changes */
	effectors.update();

	/* Check for problems */
	wd.check();
}