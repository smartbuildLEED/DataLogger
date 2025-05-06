#include <FS.h>
#include <SD.h>
#include <SPI.h>

#define SD_CS 5  // Adjust if using a different CS pin

void listFiles(fs::FS &fs, const char *dirname, uint8_t levels) {
    Serial.printf("Listing directory: %s\n", dirname);
    File root = fs.open(dirname);
    if (!root) {
        Serial.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        Serial.print("FILE: ");
        Serial.println(file.name());
        file = root.openNextFile();
    }
}

void setup() {
    Serial.begin(115200);
    if (!SD.begin(SD_CS)) {
        Serial.println("SD Card Mount Failed");
        return;
    }
    Serial.println("SD Card Initialized");
    
    listFiles(SD, "/", 0);  // Lists files in the root directory
}

void loop() {}