
#include <Arduino.h>

#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

extern "C"
{
  #include "user_interface.h"
}

#include "sdcard.hpp"
#include "audio.hpp"
#include "input.hpp"

// #include <SoftwareSerial.h>
// SoftwareSerial // serialCmd(D1, D2, false, 2 * 16);

#define mp3_buf_max 1024 * 15
static uint8_t mp3_buf[mp3_buf_max] = {};

void config_wifi()
{
  Serial.println("Config to WiFi");

  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);

  WiFi.begin(SSID.c_str(), PASSWORD.c_str());

}

void setup()
{
  system_update_cpu_freq(SYS_CPU_160MHZ);

  pinMode(D0, OUTPUT);

  // serialCmd.begin(115200);
  // serialCmd.println("Connecting to Control");

  Serial.begin(115200);
  delay(1000);
  Serial.println("Jh Voice");

  if (sdcard_init()) {
    if (sdcard_load_wifi()) {
      Serial.printf("SSID %s PASSWORD %s\n", SSID.c_str(), PASSWORD.c_str());
    }
  }
  else
  {
    Serial.printf("SSID %s PASSWORD %s\n", SSID.c_str(), PASSWORD.c_str());
  }
  
  config_wifi();
  audio_init();
  input_init();
}

void wifi_loop() {
  // Try forever
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi");
  }
  else {
    Serial.println("Connected");
  }
}

void input_loop() {
  digitalWrite(D0, LOW);
  // if (serialCmd.available())
  String result = input_check();
  if (result.length() != 0)
  {
    // String tmp = serialCmd.readString();
    Serial.printf("play ready "), Serial.println(result);
    if (SD.exists(result)) {
      File fe = SD.open(result);
      if(fe.isFile()) {
        uint8_t *tmp = &mp3_buf[0];
        while (fe.available()) {
          *tmp = fe.read(), tmp++;
        }
        fe.close();
        in = new AudioFileSourcePROGMEM(mp3_buf, tmp - mp3_buf);

        digitalWrite(D0 , HIGH);
        mp3->begin(in, out);
      }
    } else {
      char tmp_url[0xFF] = {0};
      sprintf(tmp_url, "http://dict.youdao.com/dictvoice?audio=%s,fin&type=1", result.c_str());
      Serial.printf(tmp_url);

      http_mp3->open(tmp_url);
      int recvlen = get_http_mp3(mp3_buf, sizeof(mp3_buf), http_mp3);
      in->open(mp3_buf, http_mp3->getSize());

      digitalWrite(D0 , HIGH);
      mp3->begin(in, out);
    
      File mp3 = SD.open(result, FILE_WRITE);
      mp3.write(mp3_buf, http_mp3->getSize());
      mp3.close();
    }
  }
}

void loop()
{
  if (audio_loop()) {
    input_loop();
    wifi_loop();
    delay(1000);
  }
}
