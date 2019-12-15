
#include "AudioFileSourceHTTPStream.h"
#include "AudioFileSourceBuffer.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourcePROGMEM.h"

AudioGeneratorMP3 *mp3;
AudioFileSourceHTTPStream *http_mp3;
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
  // Serial.printf("METADATA(%s) '%s' = '%s'\n", ptr, s1, s2);
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
  // Serial.printf("STATUS(%s) '%d' = '%s'\n", ptr, code, s1);
  Serial.flush();
}

int get_http_mp3(uint8_t *buffer, uint32_t buflen, AudioFileSourceHTTPStream *file)
{
  file->SetReconnect(true, 1000);

  // Download
  int recvlen = 0, recv_tmp = 0;
  do
  {
    recv_tmp = file->read(buffer + recvlen, buflen);
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

#include "samplemp3.h"

void audio_init()
{
  audioLogger = &Serial;

  out = new AudioOutputI2S(0, true);
  mp3 = new AudioGeneratorMP3();
  mp3->RegisterStatusCB(StatusCallback, (void *)"mp3");

  http_mp3 = new AudioFileSourceHTTPStream();
  in = new AudioFileSourcePROGMEM();

  // digitalWrite(D0 , HIGH);
  // in = new AudioFileSourcePROGMEM(samplemp3, sizeof(samplemp3));
  // mp3->begin(in, out);
}

bool audio_loop()
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

    return false;
  }
  
  return true;
}
