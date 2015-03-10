// ---------------------------------------------------------------------------
// Example NewPing library sketch that does a ping about 20 times per second.
// ---------------------------------------------------------------------------

#include <NewPing.h>

#define SONAR_TRIGGER_PIN  4  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define SONAR_ECHO_PIN     2  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define SONAR_INTNO 0
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

// NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.


int lps = 0;
unsigned long tfc = 1000;
byte distance;
char sonarUpdateStep = 0;
void sonarInit(){
  pinMode(SONAR_TRIGGER_PIN, OUTPUT);
  digitalWrite(SONAR_TRIGGER_PIN, LOW);
  pinMode(SONAR_ECHO_PIN, INPUT);
  attachInterrupt(SONAR_INTNO, sonarInt, CHANGE);
  sonarUpdateStep = 0;
}
unsigned long sonarEchoStart = 0;
unsigned long sonarEchoStop = 0;
unsigned long intc = 0;
uint8_t sonarDistance = 0;
unsigned long sonarDistanceLastUpdate = 0;
void sonarInt(){
  intc++;
  if(sonarUpdateStep == 1 && digitalRead(SONAR_ECHO_PIN)==HIGH){
    sonarEchoStart = micros();
    sonarUpdateStep++;
  } else {
    if(sonarUpdateStep == 2 && digitalRead(SONAR_ECHO_PIN)==LOW){
      sonarEchoStop = micros();
      sonarUpdateStep++;
    }
  }
}
void sonarUpdate(){
  switch (sonarUpdateStep) {
      case 0: // Send trigger
        sonarUpdateStep++;
        digitalWrite(SONAR_TRIGGER_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(SONAR_TRIGGER_PIN, LOW);
        sonarEchoStart = micros();
        sonarEchoStop = 0;
        break;
      // case 1:
      //   if(sonarEchoStart < micros() - 16000)sonarUpdateStep = 0;
      //   break;
      // case 2:
      //   if(sonarEchoStart < micros() - 16000)sonarUpdateStep = 0;
      //   break;
      case 3:
        sonarDistance = (sonarEchoStop - sonarEchoStart)/57;
        sonarDistanceLastUpdate = millis();
        sonarUpdateStep = 0;
        break;
  }
}

void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  sonarInit();
}
void loop() {
  lps++;
  // distance = sonar.ping()/US_ROUNDTRIP_CM;
  if(millis() > tfc){
    Serial.print(intc);
    Serial.print(' ');
  	Serial.print(sonarDistance);
    Serial.print(' ');
    Serial.print(sonarDistanceLastUpdate);
  	Serial.print(' ');
  	Serial.println(lps);
  	tfc = millis() + 1000;
  	lps = 0;
    intc =0;
  }
  sonarUpdate();
}
