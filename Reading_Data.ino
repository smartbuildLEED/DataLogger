#include <Arduino.h>
#include "FS.h"
#include <LittleFS.h>

#define FORMAT_LITTLEFS_IF_FAILED true

char filename[] = "/TempData.txt"; 
bool fileexists = LittleFS.exists(filename);

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

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}



void setup() {
// put your setup code here, to run once:
Serial.begin(115200);
// Check if the file already exists to prevent overwritting existing data
bool fileexists = LittleFS.exists(filename);
Serial.print(fileexists);

if(!fileexists) {
  Serial.println("File doesn’t exist");  
  Serial.println("Creating file...");
  // Create File and add header
  writeFile(LittleFS, "/TempData.txt", "MY ESP32 DATA \r\n");

}
else {
  Serial.println("File already exists");

}

readFile(LittleFS, "/TempData.txt");

}

void loop() {
  // put your main code here, to run repeatedly:
  //appendFile(LittleFS, "/test.txt", ("test "));
if(!fileexists) {
  Serial.println("File doesn’t exist");  
  Serial.println("Creating file...");
  // Create File and add header
  writeFile(LittleFS, "/TempData.txt", "MY ESP32 DATA \r\n"); 
  fileexists = 1;
}
else {
  Serial.println("File already exists");
}

  
  readFile(LittleFS, "/TempData.txt");  // Read the contents of the file
  delay(5000);
}
