
#include <Arduino.h>

ADC_MODE(ADC_TOUT)

uint8_t pin_key = D3;
uint8_t pin_inh = D0;
uint8_t pin_A = D10;
uint8_t pin_B = D1;
uint8_t pin_C = D2;

// //uint16_t channel_switch(uint8_t channel_numbel);
char channel_switch(uint8_t channel_numbel);

void input_init()
{
  pinMode(pin_inh, OUTPUT);
  pinMode(pin_A, OUTPUT);
  pinMode(pin_B, OUTPUT);
  pinMode(pin_C, OUTPUT);
  pinMode(pin_key, INPUT);
  digitalWrite(pin_A, LOW);
  digitalWrite(pin_B, LOW);
  digitalWrite(pin_C, LOW);
}

String input_check()
{
  static String backup;
  String info = "";

  for (char i = 0, tmp = 0; i < 8; i++)
  {
    tmp = channel_switch(i);
    if (tmp == ' ')
      break;
    info += tmp;
    // delay(10);
  }

  if (backup.length() == 0)
  {
    backup = info;
  }
  else if (info.equals(backup))
  {
    info = "";
  }
  return info;
}

//uint16_t channel_switch(uint8_t channel_numbel)
char channel_switch(uint8_t channel_numbel)
{
  uint16_t valueAdc;
  char letter;
  switch (channel_numbel)
  {
  case 0:
    digitalWrite(pin_inh, LOW);
    digitalWrite(pin_A, LOW);
    digitalWrite(pin_B, LOW);
    digitalWrite(pin_C, LOW);
    break;
  case 1:
    digitalWrite(pin_inh, LOW);
    digitalWrite(pin_A, HIGH);
    digitalWrite(pin_B, LOW);
    digitalWrite(pin_C, LOW);
    break;
  case 2:
    digitalWrite(pin_inh, LOW);
    digitalWrite(pin_A, LOW);
    digitalWrite(pin_B, HIGH);
    digitalWrite(pin_C, LOW);
    break;
  case 3:
    digitalWrite(pin_inh, LOW);
    digitalWrite(pin_A, HIGH);
    digitalWrite(pin_B, HIGH);
    digitalWrite(pin_C, LOW);
    break;
  case 4:
    digitalWrite(pin_inh, LOW);
    digitalWrite(pin_A, LOW);
    digitalWrite(pin_B, LOW);
    digitalWrite(pin_C, HIGH);
    break;
  case 5:
    digitalWrite(pin_inh, LOW);
    digitalWrite(pin_A, HIGH);
    digitalWrite(pin_B, LOW);
    digitalWrite(pin_C, HIGH);
    break;
  case 6:
    digitalWrite(pin_inh, LOW);
    digitalWrite(pin_A, LOW);
    digitalWrite(pin_B, HIGH);
    digitalWrite(pin_C, HIGH);
    break;
  case 7:
    digitalWrite(pin_inh, LOW);
    digitalWrite(pin_A, HIGH);
    digitalWrite(pin_B, HIGH);
    digitalWrite(pin_C, HIGH);
    break;
  default:
    break;
  }
  delay(1);
  valueAdc = analogRead(A0);
  if (valueAdc > 1015)
  {
    letter = 'a';
  }
  else if (valueAdc > 995)
  {
    letter = 'b';
  }
  else if (valueAdc > 965)
  {
    letter = 'c';
  }
  else if (valueAdc > 935)
  {
    letter = 'd';
  }
  else if (valueAdc > 905)
  {
    letter = 'e';
  }
  else if (valueAdc > 875)
  {
    letter = 'f';
  }
  else if (valueAdc > 840)
  {
    letter = 'g';
  }
  else if (valueAdc > 815)
  {
    letter = 'h';
  }
  else if (valueAdc > 775)
  {
    letter = 'i';
  }
  else if (valueAdc > 740)
  {
    letter = 'j';
  }
  else if (valueAdc > 700)
  {
    letter = 'k';
  }
  else if (valueAdc > 675)
  {
    letter = 'l';
  }
  else if (valueAdc > 655)
  {
    letter = 'm';
  }
  else if (valueAdc > 620)
  {
    letter = 'n';
  }
  else if (valueAdc > 590)
  {
    letter = 'o';
  }
  else if (valueAdc > 560)
  {
    letter = 'p';
  }
  else if (valueAdc > 530)
  {
    letter = 'q';
  }
  else if (valueAdc > 500)
  {
    letter = 'r';
  }
  else if (valueAdc > 465)
  {
    letter = 's';
  }
  else if (valueAdc > 440)
  {
    letter = 't';
  }
  else if (valueAdc > 400)
  {
    letter = 'u';
  }
  else if (valueAdc > 365)
  {
    letter = 'v';
  }
  else if (valueAdc > 340)
  {
    letter = 'w';
  }
  else if (valueAdc > 315)
  {
    letter = 'x';
  }
  else if (valueAdc > 285)
  {
    letter = 'y';
  }
  else if (valueAdc > 250)
  {
    letter = 'z';
  }
  else
  {
    letter = ' ';
  }
  return letter;
}
