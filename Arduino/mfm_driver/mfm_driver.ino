#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
#include <AFMotor.h>

#define PAUSE   'p'
#define RESTART 'r'
//#define REQUEST '?'
#define DONE    'd'

#define STEPS(inches) ((int)(2000.0*inches))

#define SY_OUT 30
#define SY_IN 50
#define SY_GND 52
#define SX_OUT 31
#define SX_IN 51
#define SX_GND 53
#define C_IN 10
#define C_GROUND 43

#define OFF 0x0
#define ON 0x1

/******************************Start Global Variable Defs*******************************************************/
int CONNECTED = 0;
int PAUSED = 0;
int led = 53;
int count;

//create motor objects
AF_Stepper y_axis(200,1);
AF_Stepper x_axis(200,2);

//create LCD object
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

//global variables for maintaining the data string
String data = "[";
String print_data;

//global varaibles to track the position of the tip
int x_position = 0;
int y_position = 0;
int x_passes = 31;
int y_passes = 14;
int y_done = 0;

//global variables representing the length of raster sweeps
//at some point these will hopefully be se set in the beginning by input from the LCD screen
float x_length = 0.2;
int y_length = 3;

//global variable representing the stage of operation
int stage = -1;
/********************************End Global Variable Defs*******************************************************/


//testing function
void blink(int reps, int period){
  if(reps){
    digitalWrite(led,HIGH);
    delay(period);
    digitalWrite(led,LOW);
    delay(period);
    blink(reps-1,period); 
  }
}

/*************************Start Communication Functions*****************************************/

/*
 * args: read_byte - byte to send back to the computer if connection is accepted
 * returns: none
 * checks to see if there has already been a connection made. if there hasn't been, then
 * accept the connection. if there has been, reject
 */
void accept(int read_byte){
  if(!CONNECTED){
    Serial.flush();
    //send the sent connection request byte back
    Serial.print(read_byte);
    CONNECTED = 1;
  }
  //reject
  else{
    Serial.flush();
    Serial.print(-1);
  }
  return;
}

/*
 * args: none
 * returns: none
 * blocks while waiting for a connection request over the serial port, then
 * sends the request to accept
 */
void listen() {
  char read_byte;
  int received_request=0;
  Serial.flush();
  while(!received_request){
    if(Serial.available() > 0){
      //expect an integer valued connection request
      read_byte = Serial.parseInt();
      received_request = 1;
      Serial.flush();
    }
  }
  //determine whether to accept the connection
  //accept(read_byte);
  return;
}

//void eval() {
//  if(Serial.available() > 0) {
//    //the computer has pinged the device, so interpret its request
//    char pingByte = Serial.read();
//    if(pingByte == REQUEST) {
//      Serial.flush();
//      Serial.print(REQUEST);
//      send_data();
//    }
//    else {
//      Serial.flush();
//      Serial.print(pingByte);
//    }
//  }
//  return;
//}

//int eval() {
//  if (Serial.available() > 0) {
//    char readByte = Serial.read();
//    switch(readByte) {
//      case PAUSE:
//        PAUSED = !PAUSED;
//        break;
//      case RESTART:
//        reset();
//        return 0;
//        break;
//      case REQUEST:
//      default:
//        Serial.flush();
//        break;
//    }
//    return 1;
//  }
//  Serial.flush();
//  return -1;
//}

void finish() {
  Serial.flush();
  Serial.print(DONE);
  Serial.print('\n');
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Done");
  return;
}

/*************************End Communication Functions********************************************/

/*************************Start LCD Interface Functions******************************************/
void get_scan_area() {
  boolean length_done = false;
  boolean space_done = false;
  boolean passes_done = false;
  
  boolean x_done = false;
  boolean y_done = false;
  
  lcd.begin(16, 2);
  lcd.print("Enter Scan Info");
  lcd.setBacklight(ON);
  delay(1000);
  lcd.clear();
  
  while(!(length_done && space_done && passes_done)) {
    if(!length_done){
      lcd.setCursor(0,0);
      lcd.print(F("Pass Length:"));
      lcd.setCursor(0,1);
      lcd.print(y_length);
    }
    else if(!space_done){
      lcd.setCursor(0,0);
      lcd.print(F("Pass Spacing:"));
      lcd.setCursor(0,1);
      lcd.print(x_length);
    }
    else if(!passes_done){
      lcd.setCursor(0,0);
      lcd.print(F("Passes:"));
      lcd.setCursor(0,1);
      lcd.print(y_passes);
    }
    
    uint8_t buttons = lcd.readButtons();

    if (buttons) {
      if (buttons & BUTTON_UP) {
        if(!length_done) y_length = min(12,y_length+1);
        else if(!space_done) x_length = min(12.0,x_length+0.05);
        else if(!passes_done) y_passes++;
        delay(500);
      }
      if (buttons & BUTTON_DOWN) {
        if(!length_done) y_length = max(1,y_length-1) ;
        else if(!space_done) x_length = max(0.1,x_length-0.05);
        else if(!passes_done) y_passes = max(1,y_passes-1);
        delay(500);
      }
      if (buttons & BUTTON_SELECT) {
        lcd.clear();
        if(!length_done) length_done = true;
        else if(!space_done) space_done = true;
        else if(!passes_done) passes_done = true;
        delay(500);
      }
    }
  }
  lcd.setCursor(0,0);
  lcd.print(F("Spacing:")); lcd.print(x_length); lcd.print(" ");
  lcd.setCursor(0,1);
  lcd.print(F("Length:")); lcd.print(y_length);
  
  return;
}

int lcd_on_state(boolean write_override){
  boolean to_write = false;
  
  if(write_override) lcd.clear();
  
  uint8_t buttons = lcd.readButtons();
  
  if(buttons){
    if(buttons & BUTTON_UP) {
      lcd.clear();
      PAUSED = !PAUSED;
      to_write = true;
    }
    if(buttons & BUTTON_DOWN){
      lcd.clear();
      Serial.print(RESTART);
      Serial.print('\n');
      reset();
      return -1; 
    }
  }
  
  lcd.setCursor(0,0);
  if(!PAUSED && (to_write || write_override)){
    lcd.print(F("Up: Pause"));
    lcd.setCursor(0,1);
    lcd.print(F("Down: Reset"));
    delay(100);
  }
  else if(to_write || write_override){
    lcd.print(F("Paused!"));
    lcd.setCursor(0,1);
    lcd.print(F("Up: Unpause"));
    delay(100);
  }
  
  return 1; 
}

/*************************End LCD Interface Functions********************************************/

/*************************Start Data Management Functions****************************************/
/*
 * args: x/y/z - integer value for motor position and analog input
 * returns: none
 * updates the global data string to contain a new data point
 */
void update_data(int x, int y, int z) {
  data = data+"("+x+","+y+","+z+")"+",";
  print_data = data;
  print_data[print_data.length()-1] = ']';
  return;
}

/*
 * args: none
 * returns: none
 * prints the data string over the serial port, and then clears it
 */
void send_data() {
  if(data.length() <= 1) return;
  Serial.println(print_data);
  //Serial.print('\n');
  data = "[";
  print_data = "";
  return;
}

void write_data2(int x, int y, int z){
  Serial.print(x); Serial.print(' ');
  Serial.print(y); Serial.print(' ');
  Serial.print(z);
  Serial.print('\n');
  return;
}

void write_data(int x, int y, int z){
    Serial.print('(');
    Serial.print(x);
    Serial.print(',');
    Serial.print(y);
    Serial.print(',');
    Serial.print(z);
    Serial.print(')');
    Serial.print('\n');
    return;
}

int get_data() {
  int val = analogRead(C_IN);
  return val;
}

/*************************End Data Management Functions******************************************/

/*************************Start Motor Movement Functions*****************************************/

/*
 * args: steps - number of steps to move
 *       dir   - direction to move in
 * returns: number of steps successfully moved
 * move steps many steps along the y-axis in direction dir (+/- 1)
 */
int y_move(int steps, int dir) {
  if (dir > 0) y_axis.step(steps,FORWARD,DOUBLE);
  else y_axis.step(steps,BACKWARD,DOUBLE);
  y_axis.release();
  return steps;
}

/*
 * args: steps - number of steps to move
 *       dir   - direction to move in
 * returns: number of steps successfully moved
 * move steps many steps along the x-axis in direction dir (+/- 1)
 */
int x_move(int steps,int dir) {
  if (dir > 0) x_axis.step(steps,FORWARD,DOUBLE);
  else x_axis.step(steps,BACKWARD,DOUBLE);
  x_axis.release();
  return steps;
}

/*
 * args: inches - number of inches to move
 *       dir    - direction to move in
 * returns: 1 on success, -1 on short count
 * moves inches many inches, taking readings every interval_size, which
 * is an internal parameter to the function
 */
int y_pass(float inches, int dir) {
  int height;
  int moved = 0;
  int steps = STEPS(inches);
  
  //number of steps to take before making a reading
  int interval_size = 50; //one tenth of a rotation. can probably extend
  
  while (moved < steps) {
    if(!PAUSED){
      //turn the motor and update its position
      moved += y_move(interval_size,dir);
      y_position += dir*interval_size;
    
      //read in data and add it to the global data string
      height = get_data();
      //update_data(x_position,y_position,height);
    
      //if(!eval()) return -1;
    
      write_data2(x_position,y_position,height);
    }
     if(lcd_on_state(false) < 0) return -1;
    //else if (!eval()) return -1;
  }
  y_done++;
  return 1;
}

int x_pass(float inches, int dir) {
  int height;
  int moved = 0;
  int steps = STEPS(inches);
  
  int interval_size = 50;
  
  while (moved < steps) {
    if(!PAUSED) {
      moved += x_move(interval_size,dir);
      x_position += -1*dir*interval_size;
    
      height = get_data();
      //update_data(x_position,y_position,height);
    
      //if (!eval()) return -1;
      write_data2(x_position,y_position,height);
    }
    if (lcd_on_state(false) < 0) return -1;
    //else if (!eval()) return -1;   
  }
  return 1;
}

/*
 * args: none
 * returns: 1 on done, -1 on premature stop
 * moves the tip to the starting position
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
    x_move(20,1);
  }
  if(moved_x || moved_y) return -1;
  else{
    lcd_on_state(true);
    return 1;
  }
}

/*************************End Communication Functions********************************************/

/*************************Start Control Functions************************************************/
void setup() {
  //LED pin for testing
  pinMode(led, OUTPUT);
  pinMode(led-1,OUTPUT);
  digitalWrite(led-1,LOW);
  
  //start reading from the serial port
  Serial.begin(9600);
  Serial.flush();
  
  //set up pins for the motors and the circuit
  pinMode(SX_OUT,OUTPUT);
  pinMode(SY_OUT,OUTPUT);
  pinMode(SY_GND,OUTPUT);
  pinMode(SY_IN,INPUT);
  pinMode(SX_IN,INPUT);
  pinMode(SX_GND,OUTPUT);
  pinMode(C_GROUND,OUTPUT);
  digitalWrite(C_GROUND,LOW);
  digitalWrite(SY_GND,LOW);
  digitalWrite(SX_GND,LOW);
  
  //set the motor speeds
  y_axis.setSpeed(100);
  x_axis.setSpeed(100);
  
  //have the user input scan parameters
  get_scan_area();
  
  //listen for a connection request from the computer
  listen();
}

void loop(){
  char input;
  if(stage < 0) stage = find_start();
  
  else{
    while(y_done < y_passes) {
      if (y_pass(y_length,1) < 0) return;
      if(y_done >= y_passes) break;
      
      if (x_pass(x_length,-1) < 0) return;
      
      if (y_pass(y_length,-1) < 0) return;
      if(y_done >= y_passes) break;
      
      if (x_pass(x_length,-1) < 0) return;
    }
    blink(2,1000);
    if(y_done == y_passes) {
      finish();
      y_done ++;
    }
    //else if(y_done > y_passes) {
      //eval(); 
    //}
  }
}

void reset() {
  PAUSED = 0;
  y_done = 0;
  x_position = 0;
  y_position = 0;
  stage = -1;
  get_scan_area();
  return;
}
/*************************End Control Functions*******************************************/
