
#include <AFMotor.h>

AF_Stepper motor1(200, 1);
AF_Stepper motor2(200, 2);
int pin = 24;
void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  motor1.setSpeed(100);  // 10 rpm 
  motor2.setSpeed(100);  
  pinMode(24,INPUT);
  pinMode(34,INPUT);
  digitalWrite(24,HIGH);
  digitalWrite(45,HIGH);
}

void loop() {
  switch (digitalRead(34)) {
    case LOW: {
      motor2.step(1,FORWARD,DOUBLE);
      break;
    } 
    case HIGH: 
      motor2.release();
      break;
  }
  switch (digitalRead(53)) {
    case LOW: {
      motor2.step(1,BACKWARD,DOUBLE);
      break;
    }
    case HIGH: 
      motor2.release();
      break;
  }
}
