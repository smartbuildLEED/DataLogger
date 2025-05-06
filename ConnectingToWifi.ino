#include <WiFi.h>
#include "time.h"

// const char* ssid = "york-connect";

const char* ssid = "NETGEAR52"; // wifi network name
const char* password = "jaggedmoon842";


const char* ntpServer = "pool.ntp.org"; // server
const long gmtOffset_sec = -18000; // timezone offset = TimezoneOffset x 60 x 60
const int daylightOffset_sec = 3600; // daylight savings 

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    Serial.println(WiFi.status()); // testing connection
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void setup() {
  Serial.begin(115200);

  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password); // connecting to wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

  //init and get the time
  delay(70000);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void loop() {
  delay(1000);
  printLocalTime();
}