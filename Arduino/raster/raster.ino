#include <AFMotor.h>
#define INCHES(i) (2000*i)
#define SY_OUT 30
#define SY_IN 50
#define SX_OUT 31
#define SX_IN 51
#define C_IN 10
#define C_GROUND 43

int paused=0;
int pin = 35;
int pin_in = 39;
int stage = -2;
AF_Stepper y_axis(200,1);
AF_Stepper x_axis(200,2);

int x_position = 0;
int y_position = 0;
int x_passes = 4;
int y_passes = 5;
int y_done = 0;

//positive x towards motor2
//positive y towards motor1
void setup() {
  Serial.begin(9600);
  pinMode(SX_OUT,OUTPUT);
  pinMode(pin,OUTPUT);
  pinMode(pin_in,OUTPUT);
  pinMode(SY_OUT,OUTPUT);
  pinMode(SY_IN,INPUT);
  pinMode(SX_IN,INPUT);
  //pinMode(C_GROUND,OUTPUT);
 // digitalWrite(C_GROUND,LOW);
  y_axis.setSpeed(100);
  x_axis.setSpeed(100);
  digitalWrite(pin_in,LOW);
}

void loop() {
  int serial_stat;
  if(stage < -1){
     serial_stat=check_serial();
     if(serial_stat==0) stage = 0;
  } 
  else if((stage < 1) && (stage >-2))  stage = find_start();
  
  else{
    while(y_done < y_passes){
      if (y_passCheck(8,1) > 0){
        y_done++;
      } 
      if(y_done >= y_passes) break;
      
      x_pass(2,1);
      x_position+=2;
      
      if (y_passCheck(8,-1) > 0){
        y_done++;
      } 
      if(y_done >= y_passes) break;
      
      x_pass(2,1);
      x_position+=2;
    }
  }
  if (check_serial() == 2){
    y_done = 0;
    stage = 0;
  }
}

int y_passCheck(int inches, int dir){
  int height;
  int serial_stat;
  int moved = 0;
  int steps = INCHES(inches);
  while (moved < steps){
    if (!paused){
       y_move(20,dir);
       moved+=20;
       y_position+=dir*20;
       height = get_data();
       write_data(height);
    }
    serial_stat=check_serial();
    if(serial_stat==1 || serial_stat==0) paused=!paused;
    else if (serial_stat==2) return -1;
  }
  return 1;
}

int check_serial() {
  if(Serial.available() > 0){
      char readByte = Serial.read();
      if (readByte=='s') return 0;       //start
      else if (readByte=='p') return 1;  //pause
      else if (readByte=='r') return 2;  //restart
      Serial.flush();
  }
  else return -1;
}

int write_data(int data) {
  if(data >= 0){
    Serial.print('(');
    Serial.print(x_position);
    Serial.print(",");
    Serial.print(y_position);
    Serial.print(",");
    Serial.print(data);
    Serial.print(")");
    Serial.print("\n");
  } else {
    Serial.write("-1\n");
  }
  return 0;
}

int get_data() {
  int val = analogRead(C_IN);
  return val;
}

//move steps many steps in dir direction along the x-axis
void x_move(int steps,int dir) {
  if (dir > 0) x_axis.step(steps,FORWARD,DOUBLE);
  else x_axis.step(steps,BACKWARD,DOUBLE);
  x_axis.release();
  return;
}

//move steps many steps in dir driection along the y-axis
void y_move(int steps, int dir) {
  if (dir > 0) y_axis.step(steps,FORWARD,DOUBLE);
  else y_axis.step(steps,BACKWARD,DOUBLE);
  y_axis.release();
  return;
}

//move inches many inches in dir direction (+/-) along the x axis
void x_pass(int inches, int dir) {
  if(dir > 0) x_move(INCHES(inches),1);
  else x_move(INCHES(inches),-1);
  return;
}

//move inches many inches in dir direction (+/-) along the y axis
void y_pass(int inches, int dir) {
  if (dir > 0) y_move(INCHES(inches),1);
  else y_move(INCHES(inches),-1);
  return;
}

/* 
 * setup the microscope:
 * run until hit the switches signalling the start of the scan area
 * returns: -1 on failure, 1 on success
 */
int find_start() {
  digitalWrite(SX_OUT,HIGH);
  digitalWrite(SY_OUT,HIGH);
  digitalWrite(SX_IN,LOW);
  digitalWrite(SY_IN,LOW);
  boolean moved_x = false;
  boolean moved_y = false;
  while((digitalRead(SY_IN)==LOW)) {
    moved_y = true;
    y_move(20,-1);
  }
  while((digitalRead(SX_IN)==LOW)) {
    moved_x = true;
    x_move(20,-1);
  }
  if(moved_x || moved_y) return -1;
  else return 1;
}

