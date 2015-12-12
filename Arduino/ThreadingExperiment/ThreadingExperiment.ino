
#include <AFMotor.h>
#include <mthread.h>

AF_Stepper motor1(200, 1);
AF_Stepper motor2(200, 2);


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

  delay(1000); 
}
