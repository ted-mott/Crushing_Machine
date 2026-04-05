/*
"""
Crushing Machine code
27.03.2026
Ted M
________________________________________________________________________________________________
*/


//Libraries
//______________________________________________________________________________________________
#include <Arduino.h>
#include "crush.h"
#include <HX711.h>


//Global Variables
//______________________________________________________________________________________________
//GRBL Board
//______________________________________________________________________________________________
const int xStepPin = 2;
const int xDirPin = 5;
const int yStepPin = 3;
const int yDirPin = 6;
const int zStepPin = 4;
const int zDirPin = 7;

const int stepEnablePin = 8;

const int limitSwitchPin = 10;
//Limit Y-Axis

//Load cell
//______________________________________________________________________________________________
const int loadCellSCKPin = A0;
const int loadCellDOUTPin = A1;


//Stepper motor tracking/logic
//______________________________________________________________________________________________
unsigned long defaultStepInterval = 2000; //microseconds for full microsteps, set as fast as possible
unsigned long testStepInterval = defaultStepInterval;
const long min_stepInterval = 5000; //manually set to ensure motors do not break
unsigned long lastStep = 0;

const signed int maxStep = 10000;
//need to assign this manually
signed int defaultStepPosition = 100;
//Define as where device should return to after homing

signed int stepCount = 0;
signed int stepTarget = defaultStepPosition;
//Define as where device should return to after homing

bool limitSwitch = false;

MotorControl StepperMotor = STOP;

MachineState Mode = POSITIONING;
//commands from PC


//Classes
//______________________________________________________________________________________________
HX711 scale;


// Functions
//______________________________________________________________________________________________
void home ();
bool setPosition(int stepPosition);
void setMotorDir(MotorControl Direction);
void readSerial();
void CheckLimitSwitch();
void motorStep(unsigned long stepInterval);
void motorMove(unsigned long stepInterval);
void writeSerial();

void setup() {
  //Serial setup
  Serial.begin(115200);

  //Set PinModes for GRBL
  pinMode(xStepPin, OUTPUT);
  pinMode(xDirPin, OUTPUT);
  pinMode(yStepPin, OUTPUT);
  pinMode(yDirPin, OUTPUT);
  pinMode(zStepPin, OUTPUT);
  pinMode(zDirPin, OUTPUT);

  //Enable stepper motors
  pinMode(stepEnablePin, OUTPUT);
  digitalWrite(stepEnablePin, LOW);

  pinMode(limitSwitchPin, INPUT);

  scale.begin(loadCellDOUTPin, loadCellSCKPin);
  
  Serial.print("max Step = " + String(maxStep));
  //Send machine information

  home();
    Serial.print("Homed");
    Serial.print("\n");
}


void loop() {
  readSerial();

  switch(Mode){
    case(HOMING):
      home();
      Mode = IDLE;
      Serial.print("Homed");
      Serial.print("\n");
      break;

    case(IDLE):
      break;

    case(POSITIONING):
      if(stepCount == stepTarget){
        Mode = IDLE;
        Serial.print("moved to " + String(stepTarget));
        Serial.print("\n");
      }
      motorMove(defaultStepInterval);
      break;

    case(TESTING):
      if(stepCount > 0){
        motorMove(testStepInterval);
      }
      else{
        Mode = IDLE;
        Serial.print("Testing complete");
        Serial.print("\n");
      }
      
      break;
  }
  
  writeSerial();
}


void readSerial() {
  //set mode through serial interface.

  if (Serial.available() > 0) {
    char command = Serial.read();
    long value = Serial.parseInt();
    Serial.print(String(command) + String(value));
    Serial.print("\n");

    if(command == 'S'){
      //Stop/Idle
      StepperMotor = STOP;
      Mode = IDLE;
      Serial.print("Motor Stopped at " + String(stepCount));
      Serial.print("\n");
    }
    else if (command == 'C'){
      //Config
      testStepInterval = value;
      Serial.print("Step Interval Set:" + String(testStepInterval));
      Serial.print("\n");
    }
    else if (command == 'T' ){
      //Run test
      StepperMotor = DOWN;
      setMotorDir(StepperMotor);
      stepTarget = 0;

      Serial.print("Running Test");
      Serial.print("\n");
      Mode = TESTING;
    }
    else if (command == 'P' ){
      //Set Position
      bool setPos = setPosition(value);
      if (setPos){
        Serial.print("moving to " + String(stepTarget));
        Serial.print("\n");
        Mode = POSITIONING;
      }
      else{
        Serial.print("Error: position not acceptable");
        Serial.print("\n");
      }
    }
    else if (command == 'H' ){
      //Home
      Mode = HOMING;
    }
    else{
      Serial.print("Error: not accepted input");
      Serial.print("\n");
    }
    
  }
}


void setMotorDir(MotorControl Direction) {
  
  if(Direction == UP){
    digitalWrite(xDirPin, HIGH);
    digitalWrite(yDirPin, HIGH);
    digitalWrite(zDirPin, HIGH);
    // move motor one step, increase step count
  }
  else if(Direction == DOWN){
    //Down
    digitalWrite(xDirPin, LOW);
    digitalWrite(yDirPin, LOW);
    digitalWrite(zDirPin, LOW);
    // move motor one step, decrease step count
  }
}


void CheckLimitSwitch(){
  limitSwitch = !digitalRead(limitSwitchPin);
  //read limit switch
}


void motorStep(unsigned long stepInterval) {
  unsigned long now = micros();

  if(now - lastStep >= stepInterval){
    lastStep = now;

    digitalWrite(xStepPin, HIGH);
    digitalWrite(yStepPin, HIGH);
    digitalWrite(zStepPin, HIGH);

    delayMicroseconds(20);

    digitalWrite(xStepPin, LOW);
    digitalWrite(yStepPin, LOW);
    digitalWrite(zStepPin, LOW);
    //single pulse on both motors
    
    stepCount += StepperMotor;
    // inc or dec based on steppermotor
  }

}

void writeSerial() {
  //analogRead();

  //send values to 


}