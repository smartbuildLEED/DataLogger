#include <OneWire.h>
#include <DallasTemperature.h> 
#include <LiquidCrystal_I2C.h>

// Data wire is connected to ESP32 GPIO15
#define ONE_WIRE_BUS 4
// Setup a oneWire instance to communicate with a OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);



DeviceAddress sensor1 = { 0x28, 0xE, 0x6B, 0x36, 0x0, 0x0, 0x0, 0x2D};
DeviceAddress sensor2 = { 0x28, 0x28, 0xEB, 0x38, 0x0, 0x0, 0x0, 0xF7};
DeviceAddress sensor3 = { 0x28, 0x97, 0xD0, 0x37, 0x0, 0x0, 0x0, 0xBA};
DeviceAddress sensor4 = { 0x28, 0xC9, 0x52, 0x36, 0x0, 0x0, 0x0, 0x40};

int count = 0;

void setup() {// put your setup code here, to run once:
  // start the libary 
  sensors.begin();
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();

  
   
}

void loop() { // put your main code here, to run repeatedly:

sensors.requestTemperatures(); 


lcd.setCursor(0, 0);
lcd.print("1");

lcd.setCursor(15, 0);
lcd.print("2");

lcd.setCursor(0, 1);
lcd.print("3");

lcd.setCursor(15, 1);
lcd.print("4");


lcd.setCursor(2, 0);
lcd.print(sensors.getTempC(sensor1));


lcd.setCursor(9, 0);
lcd.print(sensors.getTempC(sensor2));

lcd.setCursor(2, 1);
lcd.print(sensors.getTempC(sensor3));

lcd.setCursor(9, 1);
lcd.print(sensors.getTempC(sensor4));

 
}
