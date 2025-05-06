#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>

// GPIO where the DS18B20 is connected to
const int oneWireBus = 4;     
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

// set LCD address
LiquidCrystal_I2C lcd(0x27, 16, 2); 

  void setup() {
    // Start the Serial Monitor
    Serial.begin(115200);
    // Start the DS18B20 sensor
    sensors.begin();

    // initialize LCD
    lcd.init();
    // turn on LCD backlight                      
    lcd.backlight();
  }

  void loop() {
    sensors.requestTemperatures(); 
    float temperatureC = sensors.getTempCByIndex(0);
    float temperatureF = sensors.getTempFByIndex(0);
    lcd.setCursor(0, 0);
    lcd.print("Temp");
    lcd.setCursor(5, 0);
    lcd.print(temperatureC);
    //lcd.println("C");
    lcd.setCursor(11, 0);
    lcd.print(temperatureF);
    //lcd.println("F");
    delay(500); 
    //lcd.clear();
  }