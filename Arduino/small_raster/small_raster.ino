
#include <AFMotor.h>

#define C_IN 10
#define C_GROUND 43

#define STEPS(inches) 2000*inches

AF_Stepper y_axis(200,1);
int y_position  = 0;

void setup() {
  Serial.begin(9600);  
  
  y_axis.setSpeed(100);
  
  pinMode(C_GROUND,OUTPUT);
  digitalWrite(C_GROUND,LOW);
}

void loop() {
  Serial.println(analogRead(C_IN));
  //y_pass(1,1);
  //y_pass(1,-1);
}

int y_pass(int inches, int dir) {
  int height;
  int moved = 0;
  int steps = STEPS(inches);
  
  int interval_size = 50;
  
  while (moved < steps) {
    moved += y_move(interval_size,dir);
    y_position += dir*interval_size;
      
    Serial.println(analogRead(C_IN));
  }
}

int y_move(int steps, int dir) {
  if (dir > 0) y_axis.step(steps,FORWARD,DOUBLE);
  else y_axis.step(steps,BACKWARD,DOUBLE);
  y_axis.release();
  return steps;
}
