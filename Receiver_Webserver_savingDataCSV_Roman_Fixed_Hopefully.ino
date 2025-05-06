#include <LiquidCrystal_I2C.h>  // LCD
#include <esp_now.h>            // Wireless comm
#include <WiFi.h>               // Wireless comm
#include "time.h"               // Real Time Clock
#include "FS.h"                 // Saving data
#include "SD.h"                 // Saving data
#include "SPI.h"                // Saving data
#include "driver/timer.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD

#define LED 2  // Internal LED on reciever

#define FORMAT_LITTLEFS_IF_FAILED true

const char* Sensor1 = "Sensor 1";
const char* Sensor2 = "Sensor 2";
const char* Sensor3 = "Sensor 3";
const char* Sensor4 = "Sensor 4";


// Real Time Clock /////////////////////////////////////////////////////////////////////
const char* ssid = "york-connect";  // wifi network name

const char* ntpServer = "pool.ntp.org";  // server
const long gmtOffset_sec = -18000;       // timezone offset = TimezoneOffset x 60 x 60
const int daylightOffset_sec = 3600;     // daylight savings

int Day = 0;
int Month = 0;
int Year = 0;
int Hour = 0;
int Min = 0;

hw_timer_t *timer0 = NULL;  //Timer variable
volatile bool timerDone = false;

void IRAM_ATTR saveToCSV()
{
  timerDone = true;
}

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    digitalWrite(LED, HIGH);
    return;
  }
  digitalWrite(LED, LOW);
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Day = timeinfo.tm_mday;
  Month = timeinfo.tm_mon;
  Year = timeinfo.tm_year + 1900;
  Hour = timeinfo.tm_hour;
  Min = timeinfo.tm_min;
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
String sensor1State = "-127";
String sensor2State = "-127";
String sensor3State = "-127";
String sensor4State = "-127";

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

void appendFile(const char* path, const char* sensor_number, float data) {
  //Serial.printf("Writing file: %s\r\n", path);

  File file = SD.open(path, FILE_APPEND);
  if (file) {
    file.print(sensor_number);
    file.print(",");
    file.println(data);
    file.close();


  } else {
    //Serial.println("Failed to open file for writing.");
  }
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
  //Serial.printf("Connecting to %s ", ssid);
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
    //Serial.println("Error initializing ESP-NOW");
    lcd.setCursor(4, 0);
    lcd.print("Error :(");
    lcd.setCursor(0, 1);
    lcd.print("Not connected :(");
    return;
  }
  // Connect to Wi-Fi network with SSID and password
  //Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(Serverssid, Serverpassword);
  IPAddress IP = WiFi.softAPIP();
  //Serial.print("AP IP address: ");
  //Serial.println(IP);
  server.begin();
  // CSV saving data //////////////////////////////////////////////////
  if (!SD.begin()) {
    //Serial.println("SD card mount failed!");
    return;
  }
  //Serial.println("SD card initialized.");

  // Optional: Write header if file doesn't exist yet
  if (SD.exists("/Data.csv")) {
    File file = SD.open("/Data.csv", FILE_APPEND);
    if (file) {
      file.println();
      file.println("Sensor,Temperature");
      file.close();
    }
  }
  /////////////////////////////////////////////////////////////////////////

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  timer0 = timerBegin(100000);            //Begin timer at 1kHz
  timerAttachInterrupt(timer0, saveToCSV);   //Register "timer0" to be the interrupt timer and "LEDOff" to be the interrupt function
  timerWrite(timer0, 0);                  //Make Sure timer starts at zero

  //Register timer0 to be the alarm to set off the timer interrupt, 500000/100000 = 5 seconds
  //alarm auto resets when timer interrupt is finished, and 0 means it auto resets indefinitely
  timerAlarm(timer0, 500000, true, 0);
}

void loop() {  // put your main code here, to run repeatedly:
  //printLocalTime();

  if (timerDone) {
    appendFile("/Data.csv", Sensor1, myData.sensor1);
    appendFile("/Data.csv", Sensor2, myData.sensor2);
    appendFile("/Data.csv", Sensor3, myData.sensor3);
    appendFile("/Data.csv", Sensor4, myData.sensor4);
  }
  timerDone = false;

  sensor1State = String(myData.sensor1);
  sensor2State = String(myData.sensor2);
  sensor3State = String(myData.sensor3);
  sensor4State = String(myData.sensor4);
  WiFiClient client = server.available();
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

            sensor1State = String(myData.sensor1);
            sensor2State = String(myData.sensor2);
            sensor3State = String(myData.sensor3);
            sensor4State = String(myData.sensor4);


            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 50px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");

            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");

            // Display current temp of sensor 1
            client.println("<p>Sensor 1: " + sensor1State + "</p>");

            // Display current temp of sensor 2
            client.println("<p>Sensor 2: " + sensor2State + "</p>");

            // Display current temp of sensor 2
            client.println("<p>Sensor 3: " + sensor3State + "</p>");
            // Display current temp of sensor 2
            client.println("<p>Sensor 4: " + sensor4State + "</p>");

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
