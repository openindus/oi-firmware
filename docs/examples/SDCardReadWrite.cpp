/**
 * @file SDCardReadWrite.cpp
 * @brief SD card read/write

 * This example shows how to read and write data to and from an SD card file

 * created   Nov 2010
 * by David A. Mellis
 * modified 9 Apr 2012
 * by Tom Igoe

 * This example code is in the public domain.
*/

#include <SPI.h>
#include <SD.h>

File myFile;

void setup() {
    // Open serial communications and wait for port to open:
    Serial.begin(115200);
  
    Serial.print("Initializing SD card...");
  
    if (!SD.begin(SD_SS)) {
        Serial.println("initialization failed!");
        return;
    }
  
    Serial.println("initialization done.");
  
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    myFile = SD.open("/test.txt", FILE_WRITE);
  
    // if the file opened okay, write to it:
    if (myFile) {
        Serial.print("Writing to test.txt...");
        myFile.println("testing 1, 2, 3.");
        // close the file:
        myFile.close();
        Serial.println("done.");
    } else {
        // if the file didn't open, print an error:
        Serial.println("error opening test.txt");
    }
  
    // re-open the file for reading:
    myFile = SD.open("/test.txt");
    if (myFile) {
        Serial.println("test.txt:");
    
        // read from the file until there's nothing else in it:
        while (myFile.available()) {
            Serial.write(myFile.read());
        }
        // close the file:
        myFile.close();
    } else {
        // if the file didn't open, print an error:
        Serial.println("error opening test.txt");
    }
}

void loop() {
    // nothing happens after setup
    delay(1000);
}