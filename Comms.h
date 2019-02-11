#ifndef COMMS_H
#define COMMS_H

void HandleSerialRead();
void HandleServerUDP();
void SendCurrRSSIloop();
void IRAM_ATTR sendLap(uint8_t Lap, uint8_t NodeAddr);

//----- other globals------------------------------
uint8_t raceMode = 0; // 0: race mode is off; 1: lap times are counted relative to last lap end; 2: lap times are relative to the race start (sum of all previous lap times);
uint8_t isSoundEnabled = 1;
uint8_t isConfigured = 0; //changes to 1 if any input changes the state of the device. it will mean that externally stored preferences should not be applied
uint8_t newLapIndex = 0;
uint8_t shouldWaitForFirstLap = 0; // 0 means start table is before the laptimer, so first lap is not a full-fledged lap (i.e. don't respect min-lap-time for the very first lap)
uint8_t sendStage = 0;
uint8_t sendLapTimesIndex = 0;
uint8_t sendLastLapIndex = 0;
uint8_t shouldSendSingleItem = 0;
uint8_t lastLapsNotSent = 0;
uint8_t thresholdSetupMode = 0;
uint32_t millisUponRequest = 0;

uint32_t RaceStartTime = 0;

#endif
