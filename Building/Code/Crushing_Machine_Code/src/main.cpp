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
#include <HX711.h>

//Global Variables
//______________________________________________________________________________________________
//GRBL Board
const int xStepPin = 2;
const int xDirPin = 5;
const int yStepPin = 3;
const int yDirPin = 6;
const int zStepPin = 4;
const int zDirPin = 7;

const int stepEnablePin = 8;

const int limitSwitchPin = 10;
bool limitSwitch = false;
//Limit Y-Axis

//Load cell
const int loadCellSCKPin = A0;
const int loadCellDOUTPin = A1;

//Stepper motor tracking
unsigned int stepCount = 0;
unsigned int stepTarget = 100;
//Define as where device should return to after homing

const int maxStep = 1000;
//need to assign this manually


unsigned long stepInterval = 8000; //microseconds for full microsteps
unsigned long lastStep = 0;


enum MotorControl {
  UP = 1,
  DOWN = -1,
  STOP = 0 
};

MotorControl StepperMotor = STOP;
//commands from PC

//Classes
//______________________________________________________________________________________________
HX711 scale;

// Functions
//______________________________________________________________________________________________
void homing ();
void setMotorDir(MotorControl Direction);
void readSerial();
void CheckLimitSwitch();
void motorStep();
void motorMove();
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

  homing(); 
}


void loop() {
  readSerial();
  motorStep();
  writeSerial();

}


void readSerial() {
  Serial.print("reading" + String(StepperMotor));
  //set mode
  // if (Serial.available() > 0) {
  //   char Mode = Serial.read();
  //   stepInterval = Serial.parseInt();

  //   if(Mode == 'u' || Mode == 'U'){
  //     //Down and go
  //     Down = true;
  //     Stop = false;
  //   }
  //   else if (Mode == 'd' || Mode == 'D'){
  //     //down and go
  //     Down = false;
  //     Stop = false;
  //   }
  //   else if (Mode == 's' || Mode == 'S'){
  //     //stop
  //     Stop = true;
  //   }
    
  // }
}

void homing(){
  StepperMotor = DOWN;
  setMotorDir(StepperMotor);
  
  while (limitSwitch == false){
    motorStep();
  }
  //move until hit limit switch

  stepCount = 0;
  Serial.print("Homed");

  StepperMotor = UP;
  setMotorDir(StepperMotor);
  //Now it is at minimum position change direction to up to move to default stepTarget
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

  if(limitSwitch){
    StepperMotor = STOP;
    //if hit limit switch at bottom stop motor
  }
}

void motorStep() {
  //single pulse on both motors

  digitalWrite(xStepPin, HIGH);
  digitalWrite(yStepPin, HIGH);
  digitalWrite(zStepPin, HIGH);

  delayMicroseconds(20);

  digitalWrite(xStepPin, LOW);
  digitalWrite(yStepPin, LOW);
  digitalWrite(zStepPin, LOW);
}


void motorMove() {
  //motor drive function
  
  CheckLimitSwitch();

  if(StepperMotor != STOP && stepCount != stepTarget){
    //if stepper motor is moving and aiming for target
    unsigned long now = micros();

    if(now - lastStep >= stepInterval){
      lastStep = now;

      motorStep();
      stepCount += StepperMotor;
      // inc or dec based on steppermotor
    }
  }
}


void writeSerial() {
  //analogRead();
  if (stepCount == stepTarget){
    Serial.print(stepCount + '\n');
  }
    
  //send values to 


}