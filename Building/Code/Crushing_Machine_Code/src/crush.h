#ifndef CRUSH_H
#define CRUSH_H

//Global Variables
//______________________________________________________________________________________________
extern unsigned long defaultStepInterval;
extern unsigned long testStepInterval;
extern const long min_stepInterval;
extern unsigned long lastStep;

extern const int maxStep;
//need to assign this manually
extern int defaultStepPosition;
//Define as where device should return to after homing

extern signed int stepCount;
extern signed int stepTarget;
//Define as where device should return to after homing

extern bool limitSwitch;

enum MotorControl {
  UP = 1,
  DOWN = -1,
  STOP = 0 
};
extern MotorControl StepperMotor;

enum MachineState {
  HOMING,
  IDLE,
  POSITIONING,
  TESTING
  //Could include error mode and config mode
};
extern MachineState Mode;


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


void home(){
  StepperMotor = DOWN;
  setMotorDir(StepperMotor);
  
  while (limitSwitch == false){
    motorMove(defaultStepInterval);
  }
  //move until hit limit switch

  stepCount = 0;

  StepperMotor = UP;
  setMotorDir(StepperMotor);
  //Now it is at minimum position change direction to up to move to default stepTarget
}


bool setPosition(int stepPosition){
  //Check position is capable of being moved to by motor

  if (0 <= stepPosition && stepPosition < maxStep){
    stepTarget = stepPosition;
    if (stepTarget > stepCount){
      //Target position is higher than current position
      StepperMotor = UP;
    }
    else if (stepTarget < stepCount){
      StepperMotor = DOWN;
    }
    setMotorDir(StepperMotor);
    return true;
  }
  else{
    return false;
  }

}


void motorMove(unsigned long stepInterval) {
  //motor drive function
  
  CheckLimitSwitch();

  if(StepperMotor != STOP && stepCount != stepTarget && !limitSwitch){
    //if stepper motor is moving and aiming for target
    motorStep(stepInterval);
  }
}


void readInput(char command, long value){

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
    Serial.print("Homing");
    stepTarget = maxStep;
  }
  else{
    Serial.print("Error: not accepted input");
    Serial.print("\n");
    }
}


#endif