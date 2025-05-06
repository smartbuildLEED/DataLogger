#include <LiquidCrystal_I2C.h>

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, 16, 2);  

void setup(){
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
}
void loop(){
  // set cursor to first column, first row
  lcd.setCursor(0, 0);
  // print message
  lcd.print("Hello Dr.Khan:D");
  delay(1000);
  // set cursor to first column, second row
  lcd.setCursor(0,1);
  lcd.print("Hi Dr.Earle :D");
  delay(1000);
   
}