#include <Arduino.h>
#include <time.h>
#include "biorb.h"

extern time_t now;

void HighLow(uint16_t highTime, uint16_t lowTime) {
  digitalWrite(IR_PIN, HIGH);
  delayMicroseconds(highTime);
  digitalWrite(IR_PIN, LOW);
  delayMicroseconds(lowTime);
}

void LowHigh(uint16_t lowTime, uint16_t highTime) {
  digitalWrite(IR_PIN, LOW);
  delayMicroseconds(lowTime);
  digitalWrite(IR_PIN, HIGH);
  delayMicroseconds(highTime);
}

void SendCode(uint32_t code) {
  uint32_t mask = 0x80000000;
  Serial.print("SendCode(0x"); Serial.print(code, HEX); Serial.print("): ");
  HighLow(9000, 4500);

  while (mask) {
    if ( code & mask )
    {
      HighLow(550, 1700);
      Serial.print("1");
    }
    else {
      Serial.print("0");
      HighLow(550, 550);
    }
    mask >>= 1;
  }
  Serial.println();
}
/*
void SendCode(String code) {
  int pos;
  if ( code.startsWith("0x") || code.startsWith("0X") ) code.remove(0, 2);

  for ( pos = 0; pos < code.length(); pos++ ) {
    char c = code.charAt(pos);
    uint8_t value = 0;
    if ( c >= '0' && c <= '9' ) value = c - '0';
    else if ( c >= 'A' && c <= 'F' ) value = c - 'A' + 10;
    else if ( c >= 'a' && c <= 'f' ) value = c - 'a' + 10;

    uint8_t mask = 0x08;
    while (mask) {
      if ( value & mask )
      {
        HighLow(550, 1700);
        Serial.print("1");
      }
      else {
        Serial.print("0");
        HighLow(550, 550);
      }
      mask >>= 1;
    }
  }
  Serial.println();
}
*/
void SetColor ( uint32_t colorCode, uint32_t BrightnessCode, bool silent ) {
  if (!silent)  Serial.print(ctime(&now));

  //  SendCode(BIORB_ON);
  SendCode(colorCode);
  if (!silent)  {
    Serial.print(": ");
    Serial.print(colorCode, HEX);
  }

  SendCode(BrightnessCode);
  if (!silent)  {
    Serial.print(", ");
    Serial.println(BrightnessCode, HEX);
  }
}
