// ConstantSpeed.pde
// -*- mode: C++ -*-
//
// Shows how to run AccelStepper in the simplest,
// fixed speed mode with no accelerations
// Requires the AFMotor library (https://github.com/adafruit/Adafruit-Motor-Shield-library)
// And AccelStepper with AFMotor support (https://github.com/adafruit/AccelStepper)
// Public domain!
/*
#include <AccelStepper.h>
#include <AFMotor.h>

AF_Stepper motor1(180, 1);


// you can change these to DOUBLE or INTERLEAVE or MICROSTEP!
void forwardstep() {  
  motor1.onestep(FORWARD, DOUBLE);
}
void backwardstep() {  
  motor1.onestep(BACKWARD, DOUBLE);
}

AccelStepper stepper(forwardstep, backwardstep); // use functions to step

void setup()
{  
   Serial.begin(9600);           // set up Serial library at 9600 bps
   Serial.println("Stepper test!");
  
   motor1.setSpeed(10);	
}

void loop()
{  
   motor1.step(100,FORWARD,INTERLEAVE);
}
*/
#include <AFMotor.h>


AF_Stepper motor1(200, 1);
AF_Stepper motor2(200, 2);
int pin = 24;
boolean read = true;
void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Stepper test!");

  
  motor1.setSpeed(20);  // 10 rpm 
  motor2.setSpeed(20);  
  //motor.step(100, FORWARD, SINGLE); 
  //motor.release();
  delay(1000);
}

void loop() {
  motor1.step(200, FORWARD, DOUBLE);
  motor2.step(200, FORWARD, DOUBLE);
  delay(1000); 
}
