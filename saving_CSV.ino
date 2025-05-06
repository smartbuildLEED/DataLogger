#include "FS.h"
#include "SD.h"
#include "SPI.h"

int counter = 0;

void setup() {
  Serial.begin(115200);

    if (!SD.begin()) {
      Serial.println("SD card mount failed!");
      return;
    }
    Serial.println("SD card initialized.");

    // Optional: Write header if file doesn't exist yet
    if (!SD.exists("/data.csv")) {
      File file = SD.open("/data.csv", FILE_WRITE);
      if (file) {
        file.println("Count,Temperature");
        file.close();
      }
    }

  // Simulate a reading
  float temp = random(200, 300) / 10.0;  // e.g., 20.0 to 30.0

  // Append to CSV
  File file = SD.open("/data.csv", FILE_APPEND);
  if (file) {
    file.print(counter);
    file.print(",");
    file.println(temp);
    file.close();
    Serial.println("Data logged.");
    counter++;
  } else {
    Serial.println("Failed to open file for writing.");
  }
}

void loop() {
  // nothing here for now
}
