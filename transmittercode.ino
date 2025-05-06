#include <OneWire.h> //temp sensor
#include <DallasTemperature.h> //temp sensor
#include <esp_now.h>// wireless comm
#include <WiFi.h>// wireless comm 

#define LED 2 // Internal LED on reciever

// TEMPERTURE SENSORS CODE///////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////


// WIRELESS COMM CODE//////////////////////////////////////////////////////// 
uint8_t broadcastAddress[] = {0xD8, 0x13, 0x2A, 0x73, 0x0C, 0xA8};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  float sensor1;
  float sensor2;
  float sensor3;
  float sensor4;
  
  
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if(status == ESP_NOW_SEND_SUCCESS){
    digitalWrite(LED,LOW); // indecates the wiether or nor the trans mission was succeussful 
  }
  else{
    digitalWrite(LED,HIGH); // failed transmission when on 
  }
}

/////////////////////////////////////////////////////////////////////////////


void setup() {// put your setup code here, to run once:
    Serial.begin(9600); // starting setrial monitor 
    sensors.begin(); // starting temp sensor
    pinMode(LED,OUTPUT);
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

    sensors.requestTemperatures(); 


    myData.sensor1 = sensors.getTempC(sensor1);
    myData.sensor2 = sensors.getTempC(sensor2);
    myData.sensor3 = sensors.getTempC(sensor3);
    myData.sensor4 = sensors.getTempC(sensor4);

    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
    delay(1000);


    
//Serial.println(sensors.getTempC(sensor1));
//Serial.println(sensors.getTempC(sensor2));
//Serial.println(sensors.getTempC(sensor3));
//Serial.println(sensors.getTempC(sensor4));
}
