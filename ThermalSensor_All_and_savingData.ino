#include <OneWire.h>
#include <DallasTemperature.h> 
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include "FS.h"
#include <LittleFS.h>
#include <esp_now.h>
#include <WiFi.h>


#define FORMAT_LITTLEFS_IF_FAILED true

// Data wire is connected to ESP32 GPIO15
#define ONE_WIRE_BUS 4
// Setup a oneWire instance to communicate with a OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

LiquidCrystal_I2C lcd(0x27, 16, 2);

DeviceAddress sensor1 = { 0x28, 0xE, 0x6B, 0x36, 0x0, 0x0, 0x0, 0x2D};
DeviceAddress sensor2 = { 0x28, 0x28, 0xEB, 0x38, 0x0, 0x0, 0x0, 0xF7};
DeviceAddress sensor3 = { 0x28, 0x97, 0xD0, 0x37, 0x0, 0x0, 0x0, 0xBA};
DeviceAddress sensor4 = { 0x28, 0xC9, 0x52, 0x36, 0x0, 0x0, 0x0, 0x40};

double  temp1, temp2, temp3, temp4; 
char Location1[] = "location1 temp: ";
char Location2[] = "location2 temp: ";
char Location3[] = "location3 temp: ";
char Location4[] = "location4 temp: ";
char units[] = "ºC";

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("- failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("- message appended");
    } else {
        Serial.println("- append failed");
    }
    file.close();
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xD8, 0x13, 0x2A, 0x73, 0x0C, 0xA8};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  double temp1;
  double temp2;
  double temp3; 
  double temp4;
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}




void setup() {// put your setup code here, to run once:
  // start the libary 
  sensors.begin();
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight(); 

  // Labels 
lcd.setCursor(0, 0);
lcd.print("1");

lcd.setCursor(15, 0);
lcd.print("2");

lcd.setCursor(0, 1);
lcd.print("3");

lcd.setCursor(15, 1);
lcd.print("4");

  // Saving data 
  Serial.begin(115200);
  if(!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
      Serial.println("LittleFS Mount Failed");
      return;
   }
   else{
       Serial.println("Little FS Mounted Successfully");
   }
   writeFile(LittleFS, "/TempData.txt", "MY ESP32 DATA \r\n");
   
  // sending data 

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  





}

void loop() { // put your main code here, to run repeatedly:

 

  for(int i = 0; i < 60; i++){
//printing the temp values to LCD
    sensors.requestTemperatures(); 
    
    lcd.setCursor(2, 0);
    lcd.print(sensors.getTempC(sensor1));
    temp1 = sensors.getTempC(sensor1);
    lcd.setCursor(9, 0);
    lcd.print(sensors.getTempC(sensor2));
    temp2 = sensors.getTempC(sensor2);
    lcd.setCursor(2, 1);
    lcd.print(sensors.getTempC(sensor3));
    temp3 = sensors.getTempC(sensor3);
    lcd.setCursor(9, 1);
    lcd.print(sensors.getTempC(sensor4));
    temp4 = sensors.getTempC(sensor4);

    myData.temp1 = temp1;
    myData.temp2 = temp2; 
    myData.temp3 = temp3; 
    myData.temp4 = temp4; 
}
  appendFile(LittleFS, "/TempData.txt", (Location1 + String(temp1)+ " " + units +"\r\n").c_str()); //Append data to the file
  appendFile(LittleFS, "/TempData.txt", (Location2 + String(temp2)+ " " + units +"\r\n").c_str()); //Append data to the file
  appendFile(LittleFS, "/TempData.txt", (Location3 + String(temp3)+ " " + units +"\r\n").c_str()); //Append data to the file
  appendFile(LittleFS, "/TempData.txt", (Location4 + String(temp4)+ " " + units +"\r\n" + "\r\n").c_str()); //Append data to the file
  
  readFile(LittleFS, "/TempData.txt"); // Read the contents of the file

   // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }







}
