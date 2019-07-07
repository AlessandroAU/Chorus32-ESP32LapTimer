#include "Utils.h"

#include <esp_attr.h>
#include <stdint.h>

uint8_t IRAM_ATTR HEX_TO_BYTE (uint8_t hi, uint8_t lo) {
  return TO_BYTE(hi) * 16 + TO_BYTE(lo);
}

uint16_t IRAM_ATTR HEX_TO_UINT16 (uint8_t * buf) {
  return (HEX_TO_BYTE(buf[0], buf[1]) << 8) + (HEX_TO_BYTE(buf[2], buf[3]));
}

int32_t IRAM_ATTR HEX_TO_SIGNED_LONG (uint8_t * buf) {
#define LEN 8
  int32_t temp = 0;
  for (int i = 0; i < LEN; i++) {
    temp += TO_BYTE(buf[LEN - 1 - i]) * (int32_t)1 << (i * 4);
  }
  return temp;
}

void IRAM_ATTR halfByteToHex(uint8_t *buf, uint8_t val) {
  buf[0] = TO_HEX((val & 0x0F));
}

void IRAM_ATTR byteToHex(uint8_t *buf, uint8_t val) {
  halfByteToHex(buf, val >> 4);
  halfByteToHex(&buf[1], val);
}

void IRAM_ATTR intToHex(uint8_t *buf, uint16_t val) {
  byteToHex(buf, val >> 8);
  byteToHex(&buf[2], val & 0x00FF);
}

void IRAM_ATTR longToHex(uint8_t *buf, uint32_t val) {
  intToHex(buf, val >> 16);
  intToHex(&buf[4], val & 0x0000FFFF);
}

float fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
