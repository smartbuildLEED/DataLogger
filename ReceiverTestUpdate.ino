#include <LiquidCrystal_I2C.h>  // LCD
#include <esp_now.h>            // Wireless comm
#include <WiFi.h>               // Wireless comm

LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD

typedef struct struct_message {

  float sensor1;
  float sensor2;
  float sensor3;
  float sensor4;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
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



void setup() {  // put your setup code here, to run once:

  // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();

  Serial.begin(9600);

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
}
