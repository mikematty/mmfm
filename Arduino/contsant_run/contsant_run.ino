#include <AFMotor.h>

#define SY_OUT 30
#define SY_IN 50
#define SY_GND 52
#define SX_OUT 31
#define SX_IN 51

int moved = 0;

AF_Stepper y_axis(200,1);
AF_Stepper x_axis(200,2);
//positive x towards motor2
//positive y towards motor1
void setup() {
  Serial.begin(9600);
  y_axis.setSpeed(100);
  x_axis.setSpeed(100);
  
  pinMode(SX_OUT,OUTPUT);
  pinMode(SY_OUT,OUTPUT);
  pinMode(SY_GND,OUTPUT);
  pinMode(SY_IN,INPUT);
  pinMode(SX_IN,INPUT);
  
  
  pinMode(43,OUTPUT);
  digitalWrite(43,LOW);
}

void loop() {
  //delay(1000);
 // y_axis.step(2000,BACKWARD,DOUBLE);
    Serial.println(analogRead(10));
//  digitalWrite(SX_OUT,HIGH);
//  digitalWrite(SY_OUT,HIGH);
//  Serial.println(digitalRead(SX_IN));
//  Serial.println(digitalRead(SY_IN));
//  delay(1000);
  //if(!moved) x_axis.step(24000,BACKWARD,DOUBLE);
//  /*else*/ x_axis.step(24000,FORWARD,DOUBLE);
//  moved = !moved; 
//  x_axis.step(4000,FORWARD,DOUBLE);
//  x_axis.release();
//  y_axis.step(4000,BACKWARD,DOUBLE);
//  y_axis.release();
//  x_axis.step(4000,BACKWARD,DOUBLE);
//  x_axis.release();
//  y_axis.step(4000,FORWARD,DOUBLE);
//  y_axis.release();
}
