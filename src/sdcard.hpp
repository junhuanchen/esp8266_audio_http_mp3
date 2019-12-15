/*
  SD card basic file example

  This example shows how to create and destroy an SD card file
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4

  created   Nov 2010
  by David A. Mellis
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*/
#include <SPI.h>
#include <SD.h>

File MusicFile;

bool sdcard_is_open = false;

bool sdcard_init()
{
  if (!SD.begin(4))
  {
    sdcard_is_open = false;
    return false;
  }
  sdcard_is_open = true;
  return true;
}

// const char SSID[32] = "zhiwu";
// const char PASSWORD[16] = "zhiwu123";
String SSID = "zhiwu";
String PASSWORD= "zhiwu123";

const char *config = "wifi.cfg";

bool sdcard_load_wifi() // "zhiwu\nzhiwu123\n"
{
  if (sdcard_is_open) {
    if (SD.exists(config)) {
      File fe = SD.open(config);
      if(fe.isFile()) {
        String tmp;
        // uint32_t size = fe.size();
        // Serial.printf("file %d size %d\n", fe.isFile(), fe.size());

        while (fe.available()) tmp += (char)fe.read();

        // Serial.println(tmp);
        int i = 0, l = 0;
        i = tmp.indexOf('\n');
        // Serial.printf("i %d\n", i);
        if (i != -1) {
          SSID = tmp.substring(0, i);
        }
        l = i + 1, i = tmp.indexOf('\n', l);
        // Serial.printf("i %d\n", i);
        if (i != -1) {
          PASSWORD = tmp.substring(l, i);
        }
        
        // Serial.printf("SSID %s PASSWORD %s\n", SSID.c_str(), PASSWORD.c_str());

        fe.close();
        return true;
      }
    }
    else {
      File cfg = SD.open(config, FILE_WRITE);
      cfg.write("zhiwu\nzhiwu123\n");
      cfg.close();
      return true;
    }
    // Serial.printf("SSID %s PASSWORD %s\n", SSID.c_str(), PASSWORD.c_str());
  }
  return false;
}

void sdcard_test()
{
  Serial.print("Initializing SD card...");

  if (!SD.begin(4))
  {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  if (SD.exists("example.txt"))
  {
    Serial.println("example.txt exists.");
  }
  else
  {
    Serial.println("example.txt doesn't exist.");
  }

  // open a new file and immediately close it:
  Serial.println("Creating example.txt...");
  MusicFile = SD.open("example.txt", FILE_WRITE);
  MusicFile.close();

  // Check to see if the file exists:
  if (SD.exists("example.txt"))
  {
    Serial.println("example.txt exists.");
  }
  else
  {
    Serial.println("example.txt doesn't exist.");
  }

  // delete the file:
  Serial.println("Removing example.txt...");
  SD.remove("example.txt");

  if (SD.exists("example.txt"))
  {
    Serial.println("example.txt exists.");
  }
  else
  {
    Serial.println("example.txt doesn't exist.");
  }
}
