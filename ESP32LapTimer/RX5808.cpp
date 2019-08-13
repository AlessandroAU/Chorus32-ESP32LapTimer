#define SPI_ADDRESS_SYNTH_B 0x01
#define SPI_ADDRESS_POWER   0x0A
#define SPI_ADDRESS_STATE   0x0F

#define PowerDownState    0b11111111111111111111
#define DefaultPowerState 0b00010000110000010011

#define ResetReg          0b00000000000000000000
#define StandbyReg        0b00000000000000000010
#define PowerOnReg        0b00000000000000000001

#include "RX5808.h"

#include "HardwareConfig.h"
#include "settings_eeprom.h"

#include <Arduino.h>
#include <stdint.h>
#include <SPI.h>
#include <driver/timer.h>

static volatile uint8_t RXBand[MaxNumReceivers];
static volatile uint8_t RXChannel[MaxNumReceivers];

void InitSPI() {
  SPI.begin(VRX_SCK, VRX_MISO, VRX_MOSI);
  delay(200);
}

void rxWrite(uint8_t addressBits, uint32_t dataBits, uint8_t CSpin) {

  uint32_t data = addressBits | (1 << 4) | (dataBits << 5);
  SPI.beginTransaction(SPISettings(1000000, LSBFIRST, SPI_MODE0));
  digitalWrite(CSpin, LOW);
  SPI.transferBits(data, NULL, 25);

  delayMicroseconds(MIN_TUNE_TIME);
  digitalWrite(CSpin, HIGH);
  SPI.endTransaction();

}

void rxWriteNode(uint8_t node, uint8_t addressBits, uint32_t dataBits) {
  switch (node) {
    case 0:
      rxWrite(addressBits, dataBits, CS1);
      break;
    case 1:
      rxWrite(addressBits, dataBits, CS2);
      break;
    case 2:
      rxWrite(addressBits, dataBits, CS3);
      break;
    case 3:
      rxWrite(addressBits, dataBits, CS4);
      break;
    case 4:
      rxWrite(addressBits, dataBits, CS5);
      break;
    case 5:
      rxWrite(addressBits, dataBits, CS6);
      break;
  }
}


void rxWriteAll(uint8_t addressBits, uint32_t dataBits) {

  uint32_t data = addressBits | (1 << 4) | (dataBits << 5);
  SPI.beginTransaction(SPISettings(1000000, LSBFIRST, SPI_MODE0));
  digitalWrite(CS1, LOW);
  digitalWrite(CS2, LOW);
  digitalWrite(CS3, LOW);
  digitalWrite(CS4, LOW);
  digitalWrite(CS5, LOW);
  digitalWrite(CS6, LOW);

  SPI.transferBits(data, NULL, 25);

  delayMicroseconds(MIN_TUNE_TIME);
  digitalWrite(CS1, HIGH);
  digitalWrite(CS2, HIGH);
  digitalWrite(CS3, HIGH);
  digitalWrite(CS4, HIGH);
  digitalWrite(CS5, HIGH);
  digitalWrite(CS6, HIGH);
  SPI.endTransaction();

}

void RXstandBy(uint8_t NodeAddr) {
  rxWriteNode(NodeAddr, SPI_ADDRESS_STATE, StandbyReg);
}

void RXpowerOn(uint8_t NodeAddr) {
  rxWriteNode(NodeAddr, SPI_ADDRESS_STATE, PowerOnReg);
}

void RXreset(uint8_t NodeAddr) {
  rxWriteNode(NodeAddr, SPI_ADDRESS_STATE, ResetReg);
}


void PowerDownAll() {
  //for (int i = 0; i < getNumReceivers(); i++) {
  //rxWrite(SPI_ADDRESS_POWER, PowerDownState, i);
  //RXstandBy(i);
  //delay(100);
  //}
  rxWriteAll(SPI_ADDRESS_POWER, PowerDownState);
}

void RXPowerDown(uint8_t NodeAddr) {
  rxWriteNode(NodeAddr, SPI_ADDRESS_POWER, PowerDownState);
}

void PowerUpAll() {
  for (int i = 0; i < getNumReceivers(); i++) {
    rxWrite(SPI_ADDRESS_POWER, DefaultPowerState, i);
  }
}

void RXPowerUp(uint8_t NodeAddr) {
  rxWriteNode(NodeAddr, SPI_ADDRESS_POWER, DefaultPowerState);
}

void SelectivePowerUp() { //powerup only the RXs that have been requested
  for (int i = 0; i < getNumReceivers(); i++) {
    RXreset(i);
    //RXstandBy(i);
    delay(50);
    RXpowerOn(i);
    //PowerUp(i);
    rxWrite(SPI_ADDRESS_POWER, DefaultPowerState, i);
//
//    Serial.print("Power up: ");
//    Serial.println(i);
  }
}

uint16_t getSynthRegisterBFreq(uint16_t f) {
  return ((((f - 479) / 2) / 32) << 7) | (((f - 479) / 2) % 32);
}


void setChannel(uint8_t channel, uint8_t NodeAddr) {
  Serial.println(channel);

  if (channel <= 7) {
    Serial.println("setChannel");
    RXChannel[NodeAddr] = channel;
    uint8_t band = RXBand[NodeAddr];
    uint16_t SetFreq = setModuleChannelBand(channel, band, NodeAddr);
    (void)SetFreq;
  }
}

void setBand(uint8_t band, uint8_t NodeAddr) {
  Serial.println(band);

  if (band <= MAX_BAND) {
    Serial.println("setBand");
    RXBand[NodeAddr] = band;
    uint8_t channel = RXChannel[NodeAddr];
    uint16_t SetFreq = setModuleChannelBand(channel, band, NodeAddr);
    (void)SetFreq;
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
  return setModuleFrequency(frequency, NodeAddr);
}

uint16_t setModuleChannelBand(uint8_t channel, uint8_t band, uint8_t NodeAddr) {
  Serial.println("setModuleChannelBand");
  Serial.print(channel);
  Serial.print(",");
  Serial.println(band);
  
  uint8_t index = channel + (8 * band);
  Serial.println(index);
  uint16_t frequency = channelFreqTable[index];
  RXBand[NodeAddr] = band;
  RXChannel[NodeAddr] = channel;
  return setModuleFrequency(frequency, NodeAddr);
}

uint16_t setModuleFrequency(uint16_t frequency, uint8_t NodeAddr) {
  rxWriteNode(NodeAddr, SPI_ADDRESS_SYNTH_B, getSynthRegisterBFreq(frequency));
  return frequency;
}

uint16_t setModuleFrequencyAll(uint16_t frequency) {
  rxWriteAll(SPI_ADDRESS_SYNTH_B, getSynthRegisterBFreq(frequency));
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
    default:
      return "";
      break;
  }
}

void setRXBand(uint8_t node, uint8_t band) {
  RXBand[node] = band;
}
uint8_t getRXBand(uint8_t node) {
  return RXBand[node];
}

void setRXChannel(uint8_t node, uint8_t channel) {
  RXChannel[node] = channel;
}

uint8_t getRXChannel(uint8_t node) {
  return RXChannel[node];
}

uint16_t getFrequencyFromBandChannel(uint8_t band, uint8_t channel) {
  if(channel >= 8 || band > MAX_BAND) {
    return 0;
  }
  uint8_t index = channel + (8 * band);
  uint16_t frequency = channelFreqTable[index];
  return frequency;
}
