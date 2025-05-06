#include <SD.h>
#include <SPI.h>
#include <sqlite3.h>

#define SD_CS 5  // Chip select pin for SD card

sqlite3 *db;  
sqlite3_stmt *stmt;
char *errMsg = 0;
char dbPath[100] = "/sensor_data";  

void createTable() {
    const char *sql = "CREATE TABLE IF NOT EXISTS Temperature (id INTEGER PRIMARY KEY, timestamp TEXT, value REAL);";
    
    if (sqlite3_exec(db, sql, NULL, NULL, &errMsg) == SQLITE_OK) {
        Serial.println("Table created successfully!");
    } else {
        Serial.printf("Error creating table: %s\n", errMsg);
        sqlite3_free(errMsg);
    }
}


void setup() {
    Serial.begin(115200);
    sqlite3_initialize();
    // Initialize SD Card
    if (!SD.begin(SD_CS)) {
        Serial.println("SD Card Mount Failed");
        return;
    }
    Serial.println("SD Card Initialized");

    // Open database file from SD Card
    if (db != NULL){
      sqlite3_close(db);
    }

    int roman = sqlite3_open(dbPath, &db);
    if (roman == SQLITE_OK) {
        Serial.println("Database opened successfully!");
        createTable();
    } else {
        Serial.println("Failed to open database!");
    }
    Serial.println(roman);
     Serial.println(sqlite3_extended_errcode(db));
}


void loop() {
//   File testFile = SD.open(dbPath, FILE_WRITE);
// if (!testFile) {
//   Serial.println("Write test failed: File could not be opened for writing");
// } else {
//   testFile.println("Testing write access");
//   testFile.close();
//   Serial.println("Write test passed");
//   SD.remove("/write_test.txt");  // Clean up
// }
}