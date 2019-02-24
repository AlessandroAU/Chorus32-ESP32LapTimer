#define SPI_ADDRESS_SYNTH_B 0x01
#define SPI_ADDRESS_POWER   0x0A
#define SPI_ADDRESS_STATE   0x0F

#include "HardwareConfig.h"

#include <Arduino.h>
#include <stdint.h>
#include <SPI.h>
#include <driver/timer.h>

uint16_t freq = 5820;

void InitSPI() {
  SPI.begin(SCK, MISO, MOSI, -1);
}

void rxWrite(uint8_t addressBits, uint32_t dataBits, uint8_t CSpin) {

  uint32_t data = addressBits | (1 << 4) | (dataBits << 5);

  SPI.beginTransaction(SPISettings(1000000, LSBFIRST, SPI_MODE0));
  digitalWrite(CSpin, LOW);
  SPI.transferBits(data, NULL, 25);
  digitalWrite(CSpin, HIGH);
  SPI.endTransaction();
  delayMicroseconds(MIN_TUNE_TIME);

}

uint16_t getSynthRegisterBFreq(uint16_t f) {
  return ((((f - 479) / 2) / 32) << 7) | (((f - 479) / 2) % 32);
}


void setChannel(uint8_t channel, uint8_t NodeAddr) {
  Serial.println(channel);

  if (channel >= 0 && channel <= 7) {
    Serial.println("setChannel");
    RXChannel[NodeAddr] = channel;
    uint8_t band = RXBand[NodeAddr];
    uint16_t SetFreq = setModuleChannelBand(channel, band, NodeAddr);
  }
}

void setBand(uint8_t band, uint8_t NodeAddr) {
  Serial.println(band);

  if (band >= 0 && band <= MAX_BAND) {
    Serial.println("setBand");
    RXBand[NodeAddr] = band;
    uint8_t channel = RXChannel[NodeAddr];
    uint16_t SetFreq = setModuleChannelBand(channel, band, NodeAddr);
  }
}

uint16_t setModuleChannelBand(uint8_t NodeAddr) {
  Serial.println("setModuleChannelBand");
  Serial.print(RXChannel[NodeAddr]);
  Serial.print(",");
  Serial.println(RXBand[NodeAddr]);

  uint8_t index = RXChannel[NodeAddr] + (8 * RXBand[NodeAddr]);
  Serial.println(index);
  uint16_t frequency = channelFreqTable[index];
  //return setModuleFrequency(frequency);

  switch (NodeAddr) {
    case 0:
      rxWrite(SPI_ADDRESS_SYNTH_B, getSynthRegisterBFreq(frequency), CS1);
      break;

    case 1:
      rxWrite(SPI_ADDRESS_SYNTH_B, getSynthRegisterBFreq(frequency), CS2);
      break;

    case 2:
      rxWrite(SPI_ADDRESS_SYNTH_B, getSynthRegisterBFreq(frequency), CS3);
      break;

    case 3:
      rxWrite(SPI_ADDRESS_SYNTH_B, getSynthRegisterBFreq(frequency), CS4);
      break;
  }


  printf("a = %" PRIu16 "\n", frequency);
  return frequency;
}

uint16_t setModuleChannelBand(uint8_t channel, uint8_t band, uint8_t NodeAddr) {
  Serial.println("setModuleChannelBand");
  Serial.print(channel);
  Serial.print(",");
  Serial.println(band);

  uint8_t index = channel + (8 * band);
  Serial.println(index);
  uint16_t frequency = channelFreqTable[index];
  //return setModuleFrequency(frequency);

  switch (NodeAddr) {
    case 0:
      rxWrite(SPI_ADDRESS_SYNTH_B, getSynthRegisterBFreq(frequency), CS1);
      break;

    case 1:
      rxWrite(SPI_ADDRESS_SYNTH_B, getSynthRegisterBFreq(frequency), CS2);
      break;

    case 2:
      rxWrite(SPI_ADDRESS_SYNTH_B, getSynthRegisterBFreq(frequency), CS3);
      break;

    case 3:
      rxWrite(SPI_ADDRESS_SYNTH_B, getSynthRegisterBFreq(frequency), CS4);
      break;
  }


  //printf("a = %" PRIu16 "\n", frequency);
  return frequency;
}

uint16_t setModuleFrequency(uint16_t frequency, uint8_t NodeAddr) {

  switch (NodeAddr) {
    case 0:
      rxWrite(SPI_ADDRESS_SYNTH_B, getSynthRegisterBFreq(frequency), CS1);
      break;

    case 1:
      rxWrite(SPI_ADDRESS_SYNTH_B, getSynthRegisterBFreq(frequency), CS2);
      break;

    case 2:
      rxWrite(SPI_ADDRESS_SYNTH_B, getSynthRegisterBFreq(frequency), CS3);
      break;

    case 3:
      rxWrite(SPI_ADDRESS_SYNTH_B, getSynthRegisterBFreq(frequency), CS4);
      break;
  }
  return frequency;
}

String getBandLabel(int band) {
  
  switch (band) {
    case 0:
      return "R";
      break;
    case 1:
      return "A";
      break;
    case 2:
      return "B";
      break;
    case 3:
      return "E";
      break;
    case 4:
      return "F";
      break;
    case 5:
      return "D";
      break;
    case 6:
      return "X";
      break;
    case 7:
      return "XX";
      break;
  }
  
}
