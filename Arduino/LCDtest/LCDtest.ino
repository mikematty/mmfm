/*********************

Example code for the Adafruit RGB Character LCD Shield and Library

This code displays text on the shield, and also reads the buttons on the keypad.
When a button is pressed, the backlight changes color.

**********************/

// include the library code:
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>

// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();


#define OFF 0x0
#define ON 0x1

int x_length = 1;
boolean x_set = false;

int y_length = 1;
boolean y_set = false;

void setup() {
  // Debugging output
  Serial.begin(9600);
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  lcd.print("Enter Scan Area Size:");
  lcd.setBacklight(ON);
  delay(1000);
  lcd.clear();
}

uint8_t i=0;
void loop() {
  // print the number of seconds since reset:
  if(!x_set){
    lcd.setCursor(0,0);
    lcd.print(F("X scan size:"));
    lcd.setCursor(0,1);
    lcd.print(x_length);
  }
  else if(!y_set){
    lcd.setCursor(0,0);
    lcd.print(F("Y scan size:"));
    lcd.setCursor(0,1);
    lcd.print(y_length);
  }
  else{
    lcd.setCursor(0,0);
    lcd.print(F("Scan size is:"));
    lcd.setCursor(0,1);
    lcd.print(F("X:")); lcd.print(x_length); 
    lcd.print(" ");
    lcd.print(F("Y:")); lcd.print(y_length);  
  }

  uint8_t buttons = lcd.readButtons();

  if (buttons) {
    if (buttons & BUTTON_UP) {
      if(!x_set) x_length++;
      else if(!y_set) y_length++;
      delay(500);
    }
    if (buttons & BUTTON_DOWN) {
      if(!x_set) x_length--;
      else if(!y_set) y_length--;
      delay(500);
    }
    if (buttons & BUTTON_SELECT) {
      lcd.clear();
      if(!x_set) x_set = true;
      else if(!y_set) y_set = true;
      delay(500);
    }
  }
}
