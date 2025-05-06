// Import required libraries
#ifdef ESP32
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#else
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#endif




#include <OneWire.h>
#include <DallasTemperature.h>


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


// Variables to store temperature values
String temperatureSensor1 = "";
String temperatureSensor2 = "";
String temperatureSensor3 = "";
String temperatureSensor4 = "";

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

// Replace with your network credentials
const char* ssid = "york-connect";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);



String readDSTemperatureCSensor1() {
  // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
  sensors.requestTemperatures();
  float temp1 = sensors.getTempCByIndex(0);
  
  if (temp1 == -127.00) {
    Serial.println("Failed to read from DS18B20 sensor");
    return "--";
  } else {
    Serial.print("Temperature Celsius: ");
    Serial.println(temp1);
    
  }
  return String(temp1);
}

String readDSTemperatureCSensor2() {
  // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
  sensors.requestTemperatures();
  float temp2 = sensors.getTempCByIndex(1);
  
  if (temp2 == -127.00) {
    Serial.println("Failed to read from DS18B20 sensor");
    return "--";
  } else {
    Serial.print("Temperature Celsius: ");
    Serial.println(temp2);
  }
  return String(temp2);
}

String readDSTemperatureCSensor3() {
  // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
  sensors.requestTemperatures();
  float temp3 = sensors.getTempCByIndex(2);
  
  if (temp3 == -127.00) {
    Serial.println("Failed to read from DS18B20 sensor");
    return "--";
  } else {
    Serial.print("Temperature Celsius: ");
    Serial.println(temp3);
    }
  return String(temp3);
}

String readDSTemperatureCSensor4() {
  // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
  sensors.requestTemperatures();
  float temp4 = sensors.getTempCByIndex(3);



  if (temp4 == -127.00) {
    Serial.println("Failed to read from DS18B20 sensor");
    return "--";
  } else {
    Serial.println(temp4);
  }
  return String(temp4);
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .ds-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>ESP DS18B20 Server</h2>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="ds-labels">Temperature Celsius</span> 
    <span id="temperaturec">%TEMPERATUREC%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="ds-labels">Temperature Fahrenheit</span>
    <span id="temperaturef">%TEMPERATUREF%</span>
    <sup class="units">&deg;F</sup>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperaturec").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperaturec", true);
  xhttp.send();
}, 10000) ;
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperaturef").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperaturef", true);
  xhttp.send();
}, 10000) ;
</script>
</html>)rawliteral";

// Replaces placeholder with DS18B20 values
String processor(const String& var) {
  //Serial.println(var);
  if (var == "TEMPERATUREC") {
    return temperatureC;
  } else if (var == "TEMPERATUREF") {
    return temperatureF;
  }
  return String();
}




void setup() {  // put your setup code here, to run once:
  // start the libary
  sensors.begin();
  Serial.begin(115200);

  temperatureS1 = readDSTemperatureCSensor1();
  temperatureS2 = readDSTemperatureCSensor2();
  temperatureS3 = readDSTemperatureCSensor3();
  temperatureS4 = readDSTemperatureCSensor4();

  // Connect to Wi-Fi
  WiFi.begin(ssid);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("connected ;D");
  Serial.println(" ");

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP()); 

   // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperaturec", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", temperatureC.c_str());
  });
  server.on("/temperaturef", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", temperatureF.c_str());
  });
  // Start server
  server.begin();

}

void loop() {  // put your main code here, to run repeatedly:

  
  // sensors.requestTemperatures();
  // lcd.setCursor(0, 0);
  // lcd.print(sensors.getTempC(sensor1));
  // lcd.setCursor(9, 0);
  // lcd.print(sensors.getTempC(sensor2));
  // lcd.setCursor(2, 1);
  // lcd.print(sensors.getTempC(sensor3));
  // lcd.setCursor(9, 1);
  // lcd.print(sensors.getTempC(sensor4));
}
