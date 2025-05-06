// Load Wi-Fi library
#include <WiFi.h>
#include <WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Replace with your network credentials
const char* ssid = "ESP32-Access-Point";
const char* password = "123456789";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// TEMPERTURE SENSORS CODE///////////////////////////////////////////////////
// Data wire is connected to ESP32 GPIO15
#define ONE_WIRE_BUS 4
// Setup a oneWire instance to communicate with a OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

DeviceAddress sensor1 = { 0x28, 0xE, 0x6B, 0x36, 0x0, 0x0, 0x0, 0x2D };
DeviceAddress sensor2 = { 0x28, 0x28, 0xEB, 0x38, 0x0, 0x0, 0x0, 0xF7 };
DeviceAddress sensor3 = { 0x28, 0x97, 0xD0, 0x37, 0x0, 0x0, 0x0, 0xBA };
DeviceAddress sensor4 = { 0x28, 0xC9, 0x52, 0x36, 0x0, 0x0, 0x0, 0x40 };
/////////////////////////////////////////////////////////////////////////////

// State of temp/////////////////////////////////////////////////////////////
String sensor1State = "-127";
String sensor2State = "-127";
String sensor3State = "-127";
String sensor4State = "-127";



void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  sensors.begin();

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.begin();
}

void loop() {
  WiFiClient client = server.available();  // Listen for incoming clients
  sensors.requestTemperatures(); 
  sensor1State = String(sensors.getTempC(sensor1));
  sensor2State = String(sensors.getTempC(sensor2));
  sensor3State = String(sensors.getTempC(sensor3));
  sensor4State = String(sensors.getTempC(sensor4));
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

            sensor1State = String(sensors.getTempC(sensor1));
            sensor2State = String(sensors.getTempC(sensor2));
            sensor3State = String(sensors.getTempC(sensor3));
            sensor4State = String(sensors.getTempC(sensor4));


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
