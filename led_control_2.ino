int latchPin = 8;
int clockPin = 12;
int dataPin = 11;

int trigPin = 4;    // Trigger
int echoPin = 5;    // Echo

int numberOfLED;

long distanceOfCM; // distance in unit cm
long duration; // original return value of distance sensor

int numOfRegisters = 2;
byte* registerState;

long effectId = 0;
long prevEffect = 0;
long effectRepeat = 0;
long effectSpeed = 30;

void setup() {
  Serial.begin (9600);
  //Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);// this part for distance sensor

  //Initialize array, this part for LED control
  registerState = new byte[numOfRegisters];
  for (size_t i = 0; i < numOfRegisters; i++) {
    registerState[i] = 0;
  }

  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  for (int i = 0; i < 16; i++) {
    regWrite(i, LOW);
  }
}

void loop() {
  distanceOfCM = getDistance();//get distance in unit cm
  LEDControl(distanceOfCM);
}

long getDistance()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
 
  // Convert the time into a distance
  long cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
  return cm;
}

void LEDControl(int distanceOfCM)
{
  //need a function to get distance in unit cm
  if (distanceOfCM > 74) {
    for (int i = 0; i < 16; i++) {
      regWrite(i, HIGH);
    }
  }

  // linear equation between LED pin number and distance. Detecting distance range between 20cm to 52cm
  numberOfLED = (distanceOfCM - 10) / 4;
  for (int j = 0; j < numberOfLED; j++) {
    regWrite(j, HIGH);
  }//turn on LED from number 0 to parameter "numberOfLED"
  for (int k = numberOfLED; k < 16; ++k) {
    regWrite(k, LOW);
  }//turn off LED from parameter "numberOfLED" to 16
}

void regWrite(int pin, bool state) {
  //Determines register
  int reg = pin / 8;
  //Determines pin for actual register
  int actualPin = pin - (8 * reg);

  //Begin session
  digitalWrite(latchPin, LOW);

  for (int i = 0; i < numOfRegisters; i++) {
    //Get actual states for register
    byte* states = &registerState[i];

    //Update state
    if (i == reg) {
      bitWrite(*states, actualPin, state);
    }

    //Write
    shiftOut(dataPin, clockPin, MSBFIRST, *states);
  }

  //End session
  digitalWrite(latchPin, HIGH);
}
