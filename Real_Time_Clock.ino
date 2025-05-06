#include <WiFi.h>
#include "time.h"

const char* ssid       = "york-connect";
//const char* password   = "";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -18000;
const int   daylightOffset_sec = 3600;

int Day = 0; 
int Month = 0;
int Year = 0; 
int Hour = 0;
int Min = 0;
int Sec = 0;

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Day = timeinfo.tm_mday;
  Month = timeinfo.tm_mon;
  Year = timeinfo.tm_year + 1900;
  Hour = timeinfo.tm_hour;
  Min = timeinfo.tm_min;
  Sec = timeinfo.tm_sec;
}

void setup()
{
  Serial.begin(115200);
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  WiFiClient client;
    if (client.connect("pool.ntp.org", 123)) {
      Serial.println("Internet is working!");
      client.stop();
    } else {
      Serial.println("No internet access!");
    }
  delay(5000);
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  delay(2000);
  printLocalTime();
  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}
void loop()
{
  delay(5000);
  printLocalTime();
  Serial.println(Day);
  Serial.println(Month);
  Serial.println(Year);
  Serial.println(Hour);
  Serial.println(Min);
  Serial.println(Sec);
}





