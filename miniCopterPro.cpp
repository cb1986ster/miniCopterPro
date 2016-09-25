#include "miniCopterPro.h"

void miniCopterPro::setup(){
	unsigned long int initCalibrationTime = millis() + 13000;

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
