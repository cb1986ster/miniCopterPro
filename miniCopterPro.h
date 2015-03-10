#ifndef _MINICOPTERPRO_H_
#define _MINICOPTERPRO_H_ 1

#include "ioWrapper.h"
#include "effectorsWrapper.h"
#include "sensorsWrapper.h"
#include "autoPilot.h"
#include "watchDog.h"

class miniCopterPro
{
	public:
		void fly();	
	private:
		ioWrapper* io;
		sensorsWrapper* sensors;
		effectorsWrapper* effectors;
		autoPilot* pilot;
		watchDog* wd;
		void setup();
		void loop();

};

void miniCopterPro::fly(){
	setup(); /* Init all stuff */
	while(1)loop(); /* Do main loop forever */
}
void miniCopterPro::setup(){
	unsigned long int initCalibrationTime = millis() + 15000;
	/* Create watchDog */
	wd = new watchDog;

	/* init IO to user */
	io = new ioWrapper;
	io->init();

	/* connect io and run */
	effectors = new effectorsWrapper;
	effectors->connectIo(io);
	effectors->init();
	
	/* connect io and run */
	sensors = new sensorsWrapper;
	sensors->connectIo(io);
	sensors->init();

	/* connect io,sensors,effectors and run */
	pilot = new autoPilot;
	pilot->connectIo(io);
	pilot->connectSensors(sensors);
	pilot->connectEffectors(effectors);
	pilot->init();

	/* WATCHDOG. Bark! Bark! */
	wd->connectEffectors(effectors);
	wd->connectIo(io);
	wd->connectPilot(pilot);
	wd->connectSensors(sensors);
	wd->initDone();

	/* Give time to calibrate all */
	while(millis()<initCalibrationTime){
		sensors->calibration();
	}
}
void miniCopterPro::loop(){
	/* read/send data from/to user */
	io->update();

	/* update sensor readings */
	sensors->update();

	/* Allow pilot to do his job */
	pilot->doJob();

	/* Now commit all changes */
	effectors->update();

	/* Check for problems */
	wd->check();
}
#endif