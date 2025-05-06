#include <LiquidCrystal_I2C.h>  // LCD
#include <esp_now.h>            // Wireless comm
#include <WiFi.h>               // Wireless comm
#include "time.h"               // Real Time Clock

LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD

#define LED 2  // Internal LED on reciever

// Real Time Clock /////////////////////////////////////////////////////////////////////
const char* ssid = "york-connect";  // wifi network name

const char* ntpServer = "pool.ntp.org";  // server
const long gmtOffset_sec = -18000;       // timezone offset = TimezoneOffset x 60 x 60
const int daylightOffset_sec = 3600;     // daylight savings

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    digitalWrite(LED, HIGH);
    return;
  }
  digitalWrite(LED, LOW);
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
////////////////////////////////////////////////////////////////////////////////////////

// Recvieing data //////////////////////////////////////////////////////////////////////
typedef struct struct_message {

  float sensor1;
  float sensor2;
  float sensor3;
  float sensor4;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));

  lcd.setCursor(0, 0);
  lcd.print("1");
  lcd.setCursor(15, 0);
  lcd.print("2");
  lcd.setCursor(0, 1);
  lcd.print("3");
  lcd.setCursor(15, 1);
  lcd.print("4");

  lcd.setCursor(2, 0);
  lcd.print(myData.sensor1);
  lcd.setCursor(9, 0);
  lcd.print(myData.sensor2);
  lcd.setCursor(2, 1);
  lcd.print(myData.sensor3);
  lcd.setCursor(9, 1);
  lcd.print(myData.sensor4);
}
////////////////////////////////////////////////////////////////////////////////////////












void setup() {  // put your setup code here, to run once:

  // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();
  pinMode(LED, OUTPUT);
  Serial.begin(115200);

  // Connecting to Wifi /////////////////////////////////////////////////////////////////
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid);  // connecting to wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

  //init and get the time
  delay(5000);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  //////////////////////////////////////////////////////////////////////////////////////

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    lcd.setCursor(4, 0);
    lcd.print("Error :(");
    lcd.setCursor(0, 1);
    lcd.print("Not connected :(");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void loop() {  // put your main code here, to run repeatedly:

  printLocalTime();

  // Serial.print("Sensor 1:");
  // Serial.print(myData.sensor1);
  // Serial.println("C");
  // Serial.print("Sensor 2:");
  // Serial.print(myData.sensor2);
  // Serial.println("C");
  // Serial.print("Sensor 3:");
  // Serial.print(myData.sensor3);
  // Serial.println("C");
  // Serial.print("Sensor 4:");
  // Serial.print(myData.sensor4);
  // Serial.println("C");

  delay(1000);
}
