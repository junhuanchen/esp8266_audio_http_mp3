// #include <Arduino.h>
// #include "AudioGeneratorMP3.h"
// #include "AudioOutputI2SDAC.h"
// #include "AudioFileSourcePROGMEM.h"
// #include "samplemp3.h"

// extern "C" {
//   #include "user_interface.h"
// }

// AudioFileSourcePROGMEM *in;
// AudioGeneratorMP3 *mp3;
// AudioOutputI2SDAC *out;

// void setup()
// {
//   system_update_cpu_freq(SYS_CPU_160MHZ);

//   Serial.begin(115200);

//   in = new AudioFileSourcePROGMEM(samplemp3, sizeof(samplemp3));
//   mp3 = new AudioGeneratorMP3();
//   out = new AudioOutputI2SDAC();
//   Serial.println("mp3 begin\n");
//   mp3->begin(in, out);
// }

// void loop()
// {
//   if (mp3->isRunning()) {
//     mp3->loop();
//   } else {
//     Serial.printf("MP3 done\n");
//     delay(1000);
//   }
// }

#include <Arduino.h>

#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include "AudioFileSourceHTTPStream.h"
#include "AudioFileSourceBuffer.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourcePROGMEM.h"

#include "judge.hpp"

extern "C"
{
#include "user_interface.h"
}
// To run, set your ESP8266 build to 160MHz, update the SSID info, and upload.

// Enter your WiFi setup here:
const char *SSID = "zhiwu";
const char *PASSWORD = "zhiwu123";

// Enter your WiFi setup here:
// const char *SSID = "weisi";
// const char *PASSWORD = "WEIsi123456";

AudioGeneratorMP3 *mp3;
AudioFileSourceHTTPStream *file;
// AudioFileSourceBuffer *buff;
AudioFileSourcePROGMEM *in;
AudioOutputI2S *out;

// Called when a metadata event occurs (i.e. an ID3 tag, an ICY block, etc.
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  (void)isUnicode; // Punt this ball for now
  // Note that the type and string may be in PROGMEM, so copy them to RAM for printf
  char s1[32], s2[64];
  strncpy_P(s1, type, sizeof(s1));
  s1[sizeof(s1) - 1] = 0;
  strncpy_P(s2, string, sizeof(s2));
  s2[sizeof(s2) - 1] = 0;
  Serial.printf("METADATA(%s) '%s' = '%s'\n", ptr, s1, s2);
  Serial.flush();
}

// Called when there's a warning or error (like a buffer underflow or decode hiccup)
void StatusCallback(void *cbData, int code, const char *string)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  // Note that the string may be in PROGMEM, so copy it to RAM for printf
  char s1[64];
  strncpy_P(s1, string, sizeof(s1));
  s1[sizeof(s1) - 1] = 0;
  Serial.printf("STATUS(%s) '%d' = '%s'\n", ptr, code, s1);
  Serial.flush();
}

#define RX_BUF_MAX 1024 * 15

static uint8_t buffer_mp3[RX_BUF_MAX] = {};

#include "samplemp3.h"

#include "sdcard.hpp"

int http_get_mp3(uint8_t *buffer, AudioFileSourceHTTPStream *file, uint32_t recv_max = RX_BUF_MAX)
{
  file->SetReconnect(true, 1000);

  // Download
  int recvlen = 0, recv_tmp = 0;
  do
  {
    recv_tmp = file->read(buffer + recvlen, recv_max);
    recvlen += recv_tmp;
  } while (recv_tmp > 0);

  // Serial.printf("file size %d %d\n", recvlen, file->getSize());

  // file->RegisterMetadataCB(MDCallback, (void *)"HTTP");

  // for (uint32_t i = 0; i < file->getSize(); i++)
  // {
  //   Serial.printf("%02X ", buffer[i]);
  // }

  return recvlen;
}

#include <SoftwareSerial.h>

// SoftwareSerial // serialCmd(D1, D2, false, 2 * 16);

void setup()
{
  system_update_cpu_freq(SYS_CPU_160MHZ);

  pinMode(16 , OUTPUT);
  digitalWrite(16 , HIGH);

  // serialCmd.begin(115200);
  // serialCmd.println("Connecting to Control");

  Serial.begin(115200);
  delay(1000);

  // Serial.println("Connecting to WiFi");

  // WiFi.disconnect();
  // WiFi.softAPdisconnect(true);
  // WiFi.mode(WIFI_STA);

  // WiFi.begin(SSID, PASSWORD);

  // // Try forever
  // while (WiFi.status() != WL_CONNECTED)
  // {
  //   Serial.println("...Connecting to WiFi");
  //   delay(1000);
  // }
  // Serial.println("Connected");

  sdcard_test();

  // judge_init();

  audioLogger = &Serial;

  // const char *URL="http://tts.youdao.com/fanyivoice?word=banana&le=eng&keyfrom=speaker-target";

  // const char *URL = "http://dict.youdao.com/dictvoice?audio=hello+everyone,ii&type=3";

  // const char *URL = "http://dict.youdao.com/dictvoice?audio=hello+everyone,fin&type=3";
  // file = new AudioFileSourceHTTPStream(URL);
  // http_get_mp3(buffer_mp3, file);

  const char *mp3_name = "hello+everyone.mp3";

  if (SD.exists(mp3_name)) {

    Serial.println("file exists.");
    File fe = SD.open(mp3_name);

    uint8_t *tmp = &buffer_mp3[0];
    uint32_t size = fe.size();

    if(fe.isFile())
    {
      Serial.printf("file %d size %d\n", fe.isFile(), fe.size());

      while (fe.available())
      {
        // Serial.printf("%02X\n", fe.read());
        *tmp = fe.read();
        tmp++;
      }
      fe.close();
    }

    // Serial.printf("buffer_mp3 size %d %d \n", size, tmp - buffer_mp3);

    in = new AudioFileSourcePROGMEM(buffer_mp3, tmp - buffer_mp3);
    // in = new AudioFileSourcePROGMEM(buffer_mp3, file->getSize());
    out = new AudioOutputI2S(0, true);
    mp3 = new AudioGeneratorMP3();
    mp3->RegisterStatusCB(StatusCallback, (void *)"mp3");
    mp3->begin(in, out);

  } else {
    Serial.println("file doesn't exist.");
  }

  // buff = new AudioFileSourceBuffer(file, 8192);
  // buff->RegisterStatusCB(StatusCallback, (void*)"buffer");
  // while(file->loop()) delay(250);

  // in = new AudioFileSourcePROGMEM(samplemp3, sizeof(samplemp3));

}

void loop()
{
  static int lastms = 0;

  // Serial.printf("Running for %d ms...\n", lastms);

  // delay(1000);

  if (mp3->isRunning())
  {
    if (in->getSize() - in->getPos() < 200)
    {
      mp3->stop();
    }

    if (millis() - lastms > 1000)
    {
      lastms = millis();
      Serial.printf("Running for %d ms...\n", lastms);
      Serial.flush();
    }

    if (!mp3->loop())
    {
      mp3->stop();
    }
  }
  else
  {
    delay(1000);

    // // if (serialCmd.available())
    // String tmp = judge_check();
    // if (tmp.length() != 0)
    // {
    //   Serial.printf("MP3 ready\n");

    //   // String tmp = serialCmd.readString();
    //   Serial.println(tmp);

    //   static char URL[0xFF] = {};

    //   sprintf(URL, "http://dict.youdao.com/dictvoice?audio=%s,fin&type=1", tmp.c_str());

    //   Serial.printf(URL);

    //   // const char *URL = "http://dict.youdao.com/dictvoice?audio=banana,fin&type=3";

    //   file->open(URL);
    //   http_get_mp3(buffer_mp3, file);
    //   in->open(buffer_mp3, file->getSize());

    //   mp3->begin(in, out);
    // }
  }
}
