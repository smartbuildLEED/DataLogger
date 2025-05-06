#include <LiquidCrystal_I2C.h>  // LCD
#include <esp_now.h>            // Wireless comm
#include <WiFi.h>               // Wireless comm
#include "time.h"               // Real Time Clock
#include "FS.h"                 // Saving data
#include "SD.h"                 // Saving data
#include "SPI.h"                // Saving data

LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD

#define LED 2  // Internal LED on reciever

#define FORMAT_LITTLEFS_IF_FAILED true

String MySensor1 = "";
String MySensor2 = "";
String MySensor3 = "";
String MySensor4 = "";

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


// Wireless Comm ///////////////////////////////////////////////////////////////////////
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

// Web server //////////////////////////////////////////////////////////////////////////
String sensor1 = "-127";
String sensor2 = "-127";
String sensor3 = "-127";
String sensor4 = "-127";

const char* Serverssid = "ESP32-Access-Point";
const char* Serverpassword = "123456789";

// Variable to store the HTTP request
String header;

// Set web server port number to 80
WiFiServer server(80);
String PDS = "off";
String CDS = "off";

/////////////////////////////////////////////////////////////////////////////////////////

// Saving Data ///////////////////////////////////////////////////////////////////////
void appendFile(fs::FS& fs, const char* path, String message) {
  Serial.printf("Appending to file: %s\r\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("- failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("- message appended");
  } else {
    Serial.println("- append failed");
  }
  file.close();
}
void writeFile(fs::FS& fs, const char* path, const char* message) {
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
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

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(Serverssid, Serverpassword);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.begin();

  if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
    Serial.println("LittleFS Mount Failed");
    return;
  } else {
    Serial.println("Little FS Mounted Successfully");
  }

  // Check if the file already exists to prevent overwritting existing data
  bool fileexists = LittleFS.exists("/Data.txt");
  Serial.print(fileexists);
  if (!fileexists) {
    Serial.println("File doesn’t exist");
    Serial.println("Creating file...");
    // Create File and add header
    writeFile(LittleFS, "/Data.txt", "MY ESP32 DATA \r\n");
  } else {
    Serial.println("File already exists");
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void loop() {  // put your main code here, to run repeatedly:

  printLocalTime();

  MySensor1 = "Sensor 1:" + String(myData.sensor1) + " C";
  MySensor2 = "Sensor 2:" + String(myData.sensor2) + " C";
  MySensor3 = "Sensor 3:" + String(myData.sensor3) + " C";
  MySensor4 = "Sensor 4:" + String(myData.sensor4) + " C";

  //appendFile(LittleFS, "/Data.txt", MyTime);
  appendFile(LittleFS, "/Data.txt", "\n");
  appendFile(LittleFS, "/Data.txt", MySensor1);
  appendFile(LittleFS, "/Data.txt", "\n");
  appendFile(LittleFS, "/Data.txt", MySensor2);
  appendFile(LittleFS, "/Data.txt", "\n");
  appendFile(LittleFS, "/Data.txt", MySensor3);
  appendFile(LittleFS, "/Data.txt", "\n");
  appendFile(LittleFS, "/Data.txt", MySensor4);
  appendFile(LittleFS, "/Data.txt", "\n \n");
  readFile(LittleFS, "/Data.txt");

  WiFiClient client = server.available();  // Listen for incoming clients

  sensor1 = String(myData.sensor1);
  sensor2 = String(myData.sensor2);
  sensor3 = String(myData.sensor3);
  sensor4 = String(myData.sensor4);
  if (client) {                     // If a new client connects,
    Serial.println("New Client.");  // print a message out in the serial port
    String currentLine = "";        // make a String to hold incoming data from the client
    while (client.connected()) {    // loop while the client's connected
      if (client.available()) {     // if there's bytes to read from the client,
        char c = client.read();     // read a byte, then
        Serial.write(c);            // print it out the serial monitor
        header += c;
        if (c == '\n') {  // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            sensor1 = String(myData.sensor1);
            sensor2 = String(myData.sensor2);
            sensor3 = String(myData.sensor3);
            sensor4 = String(myData.sensor4);

            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              PDS = "on";
              Serial.println("pervous data on");
            } else if (header.indexOf("GET /26/off") >= 0) {
              PDS = "off";
              Serial.println("pervous data off");
            } else if (header.indexOf("GET /27/on") >= 0) {
              CDS = "on";
              Serial.println("current data on");
            } else if (header.indexOf("GET /27/off") >= 0) {
              CDS = "off";
              Serial.println("current data off");
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #800080;}</style></head>");

            client.println("<body><h1>ESP32 Web Server</h1>");

            // If the PDS is off, it displays the ON button
            if (PDS == "off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">Pervous Data</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">poop</button></a></p>");
            }
            // If the CDS is off, it displays the ON button
            if (CDS == "off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">Current values</button></a></p>");
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button button2\">poop</button></a></p>");
            }
            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else {  // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}


// client.println("<p>Sensor 1: " + sensor1 + "</p>");

//             // Display current temp of sensor 2
//             client.println("<p>Sensor 2: " + sensor2 + "</p>");

//             // Display current temp of sensor 2
//             client.println("<p>Sensor 3: " + sensor3 + "</p>");
//             // Display current temp of sensor 2
//             client.println("<p>Sensor 4: " + sensor4 + "</p>");
