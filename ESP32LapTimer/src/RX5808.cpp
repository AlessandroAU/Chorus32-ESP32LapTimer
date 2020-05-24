/*
 * This file is part of Chorus32-ESP32LapTimer
 * (see https://github.com/AlessandroAU/Chorus32-ESP32LapTimer).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
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

static volatile uint8_t RXBandModule[MAX_NUM_RECEIVERS];
static volatile uint8_t RXChannelModule[MAX_NUM_RECEIVERS];

static uint32_t lastUpdate[MAX_NUM_RECEIVERS] = {0,0,0,0,0,0};

void InitSPI() {
  SPI.begin(VRX_SCK, VRX_MISO, VRX_MOSI);
  delay(200);
  // Reset all modules to ensure they come back online in case they were offline without a power cycle (pressing the reset button)
  RXResetAll();
  delay(30);
}

bool IRAM_ATTR isRxReady(uint8_t module) {
  return (micros() - lastUpdate[module]) > MIN_TUNE_TIME_US;
}

void rxWrite(uint8_t addressBits, uint32_t dataBits, uint8_t CSpin) {

  uint32_t data = addressBits | (1 << 4) | (dataBits << 5);
  SPI.beginTransaction(SPISettings(1000000, LSBFIRST, SPI_MODE0));
  digitalWrite(CSpin, LOW);
  SPI.transferBits(data, NULL, 25);

  digitalWrite(CSpin, HIGH);
  SPI.endTransaction();
}

void rxWriteNode(uint8_t node, uint8_t addressBits, uint32_t dataBits) {
  if (node < MAX_NUM_RECEIVERS) {
    lastUpdate[node] = micros();
    rxWrite(addressBits, dataBits, CS_PINS[node]);
  }
}


void rxWriteAll(uint8_t addressBits, uint32_t dataBits) {

  uint32_t data = addressBits | (1 << 4) | (dataBits << 5);
  SPI.beginTransaction(SPISettings(1000000, LSBFIRST, SPI_MODE0));
  for(int i = 0; i < MAX_NUM_RECEIVERS; i++) {
    digitalWrite(CS_PINS[i], LOW);
  }

  SPI.transferBits(data, NULL, 25);

  for(int i = 0; i < MAX_NUM_RECEIVERS; i++) {
    digitalWrite(CS_PINS[i], HIGH);
  }

  delayMicroseconds(MIN_TUNE_TIME_US);

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

void RXResetAll() {
  for (int i = 0; i < getNumReceivers(); i++) {
    RXreset(i);
  }
}


void RXPowerDownAll() {
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

void RXPowerUpAll() {
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
  if (channel <= 7) {
    RXChannelModule[NodeAddr] = channel;
    uint8_t band = RXBandModule[NodeAddr];
    uint16_t SetFreq = setModuleChannelBand(channel, band, NodeAddr);
    (void)SetFreq;
  }
}

void setBand(uint8_t band, uint8_t NodeAddr) {
  if (band <= MAX_BAND) {
    RXBandModule[NodeAddr] = band;
    uint8_t channel = RXChannelModule[NodeAddr];
    uint16_t SetFreq = setModuleChannelBand(channel, band, NodeAddr);
    (void)SetFreq;
  }
}

uint16_t setModuleChannelBand(uint8_t NodeAddr) {
  return setModuleChannelBand(RXChannelModule[NodeAddr], RXBandModule[NodeAddr], NodeAddr);
}

uint16_t setModuleChannelBand(uint8_t channel, uint8_t band, uint8_t NodeAddr) {
  uint8_t index = channel + (8 * band);
  uint16_t frequency = channelFreqTable[index];
  RXBandModule[NodeAddr] = band;
  RXChannelModule[NodeAddr] = channel;
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

void setRXBandModule(uint8_t module, uint8_t band) {
  RXBandModule[module] = band;
}
uint8_t getRXBandModule(uint8_t module) {
  return RXBandModule[module];
}

void setRXChannelModule(uint8_t module, uint8_t channel) {
  RXChannelModule[module] = channel;
}
uint8_t getRXChannelModule(uint8_t module) {
  return RXChannelModule[module];
}

uint16_t getFrequencyFromBandChannel(uint8_t band, uint8_t channel) {
  uint16_t freq = 0;
  if(band < MAX_BAND && channel < MAX_CHANNEL) {
    freq = channelFreqTable[channel + (MAX_CHANNEL+1) * band];
  }
  return freq;
}

