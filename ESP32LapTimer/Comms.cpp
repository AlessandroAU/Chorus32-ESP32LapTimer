#include "Comms.h"

#include "RX5808.h"
#include "Utils.h"
#include "HardwareConfig.h"
#include "UDP.h"
#include "settings_eeprom.h"
#include "Laptime.h"
#include "ADC.h"

///////This is mostly from the original Chorus Laptimer, need to cleanup unused functions and variables

// API in brief (sorted alphabetically):
// Req  Resp Description
// 1    1    first lap counts (opposite to prev API)
// B    B    band
// C    C    channel
// F    F    freq
// H    H    threshold setup mode
//      L    lap (response only)
// I    I    rssi monitor interval (0 = off)
// J    J    time adjustment constant
// M    M    min lap time
// R    R    race mode
// S    S    device sounds state
// T    T    threshold
// get only:
// #    #    api version #
// a    ...  all device state
// r    r    rssi value
// t    t    time in milliseconds
// v    v    voltage
//      x    end of sequence sign (response to "a")
// y    y    is module configured (response to "a")

// input control byte constants
// get/set:
#define CONTROL_WAIT_FIRST_LAP      '1'
#define CONTROL_BAND                'B'
#define CONTROL_CHANNEL             'C'
#define CONTROL_FREQUENCY           'F'
#define CONTROL_THRESHOLD_SETUP     'H'
#define CONTROL_RSSI_MON_INTERVAL   'I'
#define CONTROL_TIME_ADJUSTMENT     'J'
#define CONTROL_RACE_MODE           'R'
#define CONTROL_MIN_LAP_TIME        'M'
#define CONTROL_NUM_RECIEVERS       'N'
#define CONTROL_SOUND               'S'
#define CONTROL_THRESHOLD           'T'
// get only:
#define CONTROL_GET_API_VERSION     '#'
#define CONTROL_WILDCARD_INDICATOR  '*'
#define CONTROL_GET_ALL_DATA        'a'
#define CONTROL_GET_RSSI            'r'
#define CONTROL_GET_TIME            't'
#define CONTROL_GET_VOLTAGE         'v'
#define CONTROL_GET_IS_CONFIGURED   'y'

// output id byte constants
#define RESPONSE_WAIT_FIRST_LAP      '1'
#define RESPONSE_BAND                'B'
#define RESPONSE_CHANNEL             'C'
#define RESPONSE_FREQUENCY           'F'
#define RESPONSE_THRESHOLD_SETUP     'H'
#define RESPONSE_RSSI_MON_INTERVAL   'I'
#define RESPONSE_TIME_ADJUSTMENT     'J'
#define RESPONSE_LAPTIME             'L'
#define RESPONSE_RACE_MODE           'R'
#define RESPONSE_MIN_LAP_TIME        'M'
#define RESPONSE_SOUND               'S'
#define RESPONSE_THRESHOLD           'T'

#define RESPONSE_API_VERSION         '#'
#define RESPONSE_RSSI                'r'
#define RESPONSE_TIME                't'
#define RESPONSE_VOLTAGE             'v'
#define RESPONSE_END_SEQUENCE        'x'
#define RESPONSE_IS_CONFIGURED       'y'

// send item byte constants
// Must correspond to sequence of numbers used in "send data" switch statement
// Subsequent items starting from 0 participate in "send all data" response
#define SEND_CHANNEL            0
#define SEND_RACE_MODE          1
#define SEND_MIN_LAP_TIME       2
#define SEND_THRESHOLD          3
#define SEND_ALL_LAPTIMES       4
#define SEND_SOUND_STATE        5
#define SEND_BAND               6
#define SEND_LAP0_STATE         7
#define SEND_IS_CONFIGURED      8
#define SEND_FREQUENCY          9
#define SEND_MON_INTERVAL       10
#define SEND_TIME_ADJUSTMENT    11
#define SEND_API_VERSION        12
#define SEND_VOLTAGE            13
#define SEND_THRESHOLD_SETUP_MODE 14
#define SEND_END_SEQUENCE       15
// following items don't participate in "send all items" response
#define SEND_LAST_LAPTIMES          100
#define SEND_TIME                   101
#define SEND_CURRENT_RSSI           102
// special item that sends all subsequent items from 0 (see above)
#define SEND_ALL_DEVICE_STATE       255

//----- RSSI --------------------------------------
#define FILTER_ITERATIONS 5 // software filtering iterations; set 0 - if filtered in hardware; set 5 - if not
uint16_t rssiArr[FILTER_ITERATIONS + 1];
uint16_t rssiThreshold = 190;


uint32_t lastRSSIsent;

#define MIN_RSSI_MONITOR_INTERVAL 1 // in milliseconds
uint16_t rssiMonitorInterval = 0; // zero means the RSSI monitor is OFF
uint32_t lastRssiMonitorReading = 0; // millis when rssi monitor value was last read

#define RSSI_SETUP_INITIALIZE 0
#define RSSI_SETUP_NEXT_STEP 1

//----- Lap timings--------------------------------
uint32_t lastMilliseconds = 0;
uint32_t raceStartTime = 0;
#define MIN_MIN_LAP_TIME 1 //seconds
#define MAX_MIN_LAP_TIME 120 //seconds
#define MAX_LAPS 100

//----- Time Adjustment (for accuracy) ------------
#define INFINITE_TIME_ADJUSTMENT 0x7FFFFFFFF // max positive 32 bit signed number
// Usage of signed int time adjustment constant inside this firmware:
// * calibratedMs = readMs + readMs/timeAdjustment
// Usage of signed int time adjustment constant from outside:
// * set to zero means time adjustment procedure was not performed for this node
// * set to INFINITE_TIME_ADJUSTMENT, means time adjustment was performed, but no need to adjust
int32_t timeAdjustment = 0;

//----- read/write bufs ---------------------------
#define READ_BUFFER_SIZE 20
uint8_t readBuf[READ_BUFFER_SIZE];
uint8_t proxyBuf[READ_BUFFER_SIZE];
uint8_t readBufFilledBytes = 0;
uint8_t proxyBufDataSize = 0;

// ----------------------------------------------------------------------------

//-----------
uint8_t CurrNodeAddrAPI = 0;  //used for functions like R*# and R*a to enumerate over all node ids
uint8_t CurrNodeAddrLaps = 0;  //used for functions like R*# and R*a to enumerate over all node ids
bool holeShot[MaxNumRecievers] = {true, true, true, true, true, true}; //wait for first trigger, IE holeshot.


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
uint32_t millisUponRequest = 0;

uint32_t RaceStartTime = 0;

uint8_t thresholdSetupMode[MaxNumRecievers];
uint16_t RXfrequencies[MaxNumRecievers];

static void sendThresholdMode(uint8_t node) {
  addToSendQueue('S');
  addToSendQueue(TO_HEX(node));
  addToSendQueue(RESPONSE_THRESHOLD_SETUP);
  addToSendQueue(TO_HEX(thresholdSetupMode[node]));
  addToSendQueue('\n');
}

void commsSetup() {
  for (int i = 0; i < NumRecievers; i++) {
    setRXBand(i, EepromSettings.RXBand[i]);
    setRXChannel(i, EepromSettings.RXChannel[i]);
    RXfrequencies[i] = EepromSettings.RXfrequencies[i];
    thresholdSetupMode[i] = 0;
  }
}

void setRaceMode(uint8_t mode) {
  if (mode == 0) { // stop race

    resetLaptimes();

    raceMode = 0;
    newLapIndex = 0;
    //playEndRaceTones();
  } else { // start race in specified mode
    //holeShot = true;
    RaceStartTime = millis();
    raceMode = mode;
    raceStartTime = millis();
    lastMilliseconds = raceStartTime;
    newLapIndex = 0;
    for(uint8_t i = 0; i < NumRecievers; ++i) {
      if(thresholdSetupMode[i]) {
        thresholdSetupMode[i] = 0;
        sendThresholdMode(i);
      }
    }
    //playStartRaceTones();
  }
}


void setMinLap(uint8_t mlt) {
  if (mlt >= MIN_MIN_LAP_TIME && mlt <= MAX_MIN_LAP_TIME) {
    setMinLapTime(mlt * 1000);
  }
}

void SendMinLap(uint8_t NodeAddr) {
  addToSendQueue('S');
  addToSendQueue(TO_HEX(NodeAddr));
  addToSendQueue('M');
  addToSendQueue('0');
  addToSendQueue(TO_HEX((getMinLapTime() / 1000)));
  addToSendQueue('\n');
  isConfigured = 1;
}

void SendIsModuleConfigured() {
  for (int i = 0; i < NumRecievers; i ++) {
    addToSendQueue('S');
    addToSendQueue(TO_HEX(i));
    addToSendQueue('y');
    addToSendQueue(TO_HEX(isConfigured));
    addToSendQueue('\n');
  }
}

void SendXdone(uint8_t NodeAddr) {
  addToSendQueue('S');
  addToSendQueue(TO_HEX(NodeAddr));
  addToSendQueue('x');
  addToSendQueue('1');
  addToSendQueue('\n');
}

void SetThresholdValue(uint16_t threshold, uint8_t NodeAddr) {
  Serial.print("Setting Threshold Value: ");
  Serial.println(threshold);
  if (threshold > 340) {
    threshold = 340;
    Serial.println("Threshold was attempted to be set out of range");
  }
  // stop the "setting threshold algorithm" to avoid overwriting the explicitly set value
  if (thresholdSetupMode[NodeAddr]) {
    thresholdSetupMode[NodeAddr] = 0;
    sendThresholdMode(NodeAddr);
  }
  setRSSIThreshold(NodeAddr, threshold * 12);
  EepromSettings.RSSIthresholds[NodeAddr] = getRSSIThreshold(NodeAddr);
  setSaveRequired();
  if (threshold != 0) {
    //playClickTones();
  } else {
    //playClearThresholdTones();
  }
}

void SendMillis() {
  uint32_t CurrMillis = millis();
  uint8_t buf[8];
  longToHex(buf, CurrMillis);

  for (int i = 0; i < NumRecievers; i ++) {
    addToSendQueue('S');
    addToSendQueue(TO_HEX(i));
    addToSendQueue('t');
    addToSendQueue(buf, 8);
    addToSendQueue('\n');
  }
}

void SendThresholdValue(uint8_t NodeAddr) {
  addToSendQueue('S');
  addToSendQueue(TO_HEX(NodeAddr));
  addToSendQueue('T');
  uint8_t buf[4];
  intToHex(buf, getRSSIThreshold(NodeAddr) / 12);
  addToSendQueue(buf, 4);
  addToSendQueue('\n');
}

void SendCurrRSSI(uint8_t NodeAddr) {

  ///Calculate Averages///
  uint32_t AvgValue = 0;
  uint16_t Result = getRSSI(NodeAddr);

  //MirrorToSerial = false;  // this so it doesn't spam the serial console with RSSI updates
  addToSendQueue('S');
  addToSendQueue(TO_HEX(NodeAddr));
  addToSendQueue('r');
  uint8_t buf[4];
  intToHex(buf, Result / 12);
  addToSendQueue(buf, 4);
  addToSendQueue('\n');
  //MirrorToSerial = true;
  lastRSSIsent = millis();

}

void SendCurrRSSIloop() {
  if (rssiMonitorInterval == 0) {
    return;
  }
  if (millis() > rssiMonitorInterval + lastRSSIsent) {
    for (int i = 0; i < NumRecievers; i ++) {
      SendCurrRSSI(i);
    }
  }
}

void setupThreshold(uint8_t phase, uint8_t node) {
  // this process assumes the following:
  // 1. before the process all VTXs are turned ON, but are distant from the Chorus device, so that Chorus sees the "background" rssi values only
  // 2. once the setup process is initiated by Chorus operator, all pilots walk towards the Chorus device
  // 3. setup process starts tracking top rssi values
  // 4. as pilots come closer, rssi should rise above the value defined by RISE_RSSI_THRESHOLD_PERCENT
  // 5. after that setup expects rssi to fall from the reached top, down by FALL_RSSI_THRESHOLD_PERCENT
  // 6. after the rssi falls, the top recorded value (decreased by TOP_RSSI_DECREASE_PERCENT) is set as a threshold

  // time constant for accumulation filter: higher value => more delay
  // value of 20 should give about 100 readings before value reaches the settled rssi
  // don't make it bigger than 2000 to avoid overflow of accumulatedShiftedRssi
#define ACCUMULATION_TIME_CONSTANT 150
#define MILLIS_BETWEEN_ACCU_READS 10 // artificial delay between rssi reads to slow down the accumulation
#define TOP_RSSI_DECREASE_PERCENT 10 // decrease top value by this percent using diff between low and high as a base
#define RISE_RSSI_THRESHOLD_PERCENT 25 // rssi value should pass this percentage above low value to continue finding the peak and further fall down of rssi
#define FALL_RSSI_THRESHOLD_PERCENT 50 // rssi should fall below this percentage of diff between high and low to finalize setup of the threshold

  static uint16_t rssiLow[MaxNumRecievers];
  static uint16_t rssiHigh[MaxNumRecievers];
  static uint16_t rssiHighEnoughForMonitoring[MaxNumRecievers];
  static uint32_t accumulatedShiftedRssi[MaxNumRecievers]; // accumulates rssi slowly; contains multiplied rssi value for better accuracy
  static uint32_t lastRssiAccumulationTime[MaxNumRecievers];

  if (!thresholdSetupMode[node]) return;

  uint16_t rssi = getRSSI(node);

  if (phase == RSSI_SETUP_INITIALIZE) {
    // initialization step
    //playThresholdSetupStartTones();
    thresholdSetupMode[node] = 1;
    rssiLow[node] = rssi; // using slowRssi to avoid catching random current rssi
    rssiHigh[node] = rssiLow[node];
    accumulatedShiftedRssi[node] = rssiLow[node] * ACCUMULATION_TIME_CONSTANT; // multiply to prevent loss in accuracy
    rssiHighEnoughForMonitoring[node] = rssiLow[node] + rssiLow[node] * RISE_RSSI_THRESHOLD_PERCENT / 100;
    lastRssiAccumulationTime[node] = millis();
    sendThresholdMode(node);
  } else {
    // active phase step (searching for high value and fall down)
    if (thresholdSetupMode[node] == 1) {
      // in this phase of the setup we are tracking rssi growth until it reaches the predefined percentage from low

      // searching for peak; using slowRssi to avoid catching sudden random peaks
      if (rssi > rssiHigh[node]) {
        rssiHigh[node] = rssi;
      }

      // since filter runs too fast, we have to introduce a delay between subsequent readings of filter values
      uint32_t curTime = millis();
      if ((curTime - lastRssiAccumulationTime[node]) > MILLIS_BETWEEN_ACCU_READS) {
        lastRssiAccumulationTime[node] = curTime;
        // this is actually a filter with a delay determined by ACCUMULATION_TIME_CONSTANT
        accumulatedShiftedRssi[node] = rssi  + (accumulatedShiftedRssi[node] * (ACCUMULATION_TIME_CONSTANT - 1) / ACCUMULATION_TIME_CONSTANT);
      }

      uint16_t accumulatedRssi = accumulatedShiftedRssi[node] / ACCUMULATION_TIME_CONSTANT; // find actual rssi from multiplied value

      if (accumulatedRssi > rssiHighEnoughForMonitoring[node]) {
        thresholdSetupMode[node] = 2;
        accumulatedShiftedRssi[node] = rssiHigh[node] * ACCUMULATION_TIME_CONSTANT;
        //playThresholdSetupMiddleTones();
        sendThresholdMode(node);
      }
    } else {
      // in this phase of the setup we are tracking highest rssi and expect it to fall back down so that we know that the process is complete

      // continue searching for peak; using slowRssi to avoid catching sudden random peaks
      if (rssi > rssiHigh[node]) {
        rssiHigh[node] = rssi;
        accumulatedShiftedRssi[node] = rssiHigh[node] * ACCUMULATION_TIME_CONSTANT; // set to highest found rssi
      }

      // since filter runs too fast, we have to introduce a delay between subsequent readings of filter values
      uint32_t curTime = millis();
      if ((curTime - lastRssiAccumulationTime[node]) > MILLIS_BETWEEN_ACCU_READS) {
        lastRssiAccumulationTime[node] = curTime;
        // this is actually a filter with a delay determined by ACCUMULATION_TIME_CONSTANT
        accumulatedShiftedRssi[node] = rssi  + (accumulatedShiftedRssi[node] * (ACCUMULATION_TIME_CONSTANT - 1) / ACCUMULATION_TIME_CONSTANT );
      }
      uint16_t accumulatedRssi = accumulatedShiftedRssi[node] / ACCUMULATION_TIME_CONSTANT;

      uint16_t rssiLowEnoughForSetup = rssiHigh[node] - (rssiHigh[node] - rssiLow[node]) * FALL_RSSI_THRESHOLD_PERCENT / 100;
      if (accumulatedRssi < rssiLowEnoughForSetup) {
        rssiThreshold = rssiHigh[node] - ((rssiHigh[node] - rssiLow[node]) * TOP_RSSI_DECREASE_PERCENT) / 100;
        SetThresholdValue(rssiThreshold / 12, node); // Function expects the threshold in / 12
        thresholdSetupMode[node] = 0;
        isConfigured = 1;
        //playThresholdSetupDoneTones();
        sendThresholdMode(node);
        SendThresholdValue(node);
      }
    }
  }
}

void IRAM_ATTR sendLap(uint8_t Lap, uint8_t NodeAddr) {

  //  Serial.print("SendLap: ");
  //  Serial.println(String(Lap) + " " + String(NodeAddr));

  uint32_t RequestedLap;

  if (raceMode == 0) {
    Serial.println("RaceMode == 0 and sendlaps was called");
    return;
  }


  //  if (Lap == 1) {  ///ugh need to fix this logic at some point but it works for now
  //    RequestedLap = LapTimes[NodeAddr][Lap] - RaceStartTime;
  //  } else {
  if (raceMode == 1) {

    if (Lap == 1) {
      RequestedLap = getLaptime(NodeAddr, Lap) - RaceStartTime; // realtive mode
    } else {
      RequestedLap = getLaptime(NodeAddr, Lap) - getLaptime(NodeAddr, Lap - 1); // realtive mode
    }

  } else if (raceMode == 2) {
    RequestedLap = getLaptime(NodeAddr, Lap) - RaceStartTime;  //absolute mode

  } else {
    Serial.println("Error: Invalid RaceMode Set");
  }
  //}

  uint8_t buf1[2];
  uint8_t buf2[8];

  addToSendQueue('S');
  addToSendQueue(TO_HEX(NodeAddr));
  addToSendQueue('L');

  byteToHex(buf1, Lap - 1);
  addToSendQueue(buf1, 2);

  longToHex(buf2, RequestedLap);
  addToSendQueue(buf2, 8);

  addToSendQueue('\n');

}

void SendNumberOfnodes(byte NodeAddr) {
  for (int i = NodeAddr + 1; i <= NumRecievers + NodeAddr; i++) {
    addToSendQueue('N');
    addToSendQueue(TO_HEX(i));
    addToSendQueue('\n');
  }
}

void IRAM_ATTR SendAllLaps(uint8_t NodeAddr) {
  uint8_t Pointer = getCurrentLap(NodeAddr);
  for (uint8_t i = 0; i < Pointer; i++) {
    sendLap(i, NodeAddr);
  }
}

void SendRSSImonitorInterval(uint8_t NodeAddr) {

  addToSendQueue('S');
  addToSendQueue(TO_HEX(NodeAddr));
  uint8_t buf[4];
  addToSendQueue('I');
  intToHex(buf, rssiMonitorInterval);
  addToSendQueue(buf, 4);
  addToSendQueue('\n');
}

void SendSoundMode(uint8_t NodeAddr) {
  addToSendQueue('S');
  addToSendQueue(TO_HEX(NodeAddr));
  addToSendQueue('S');
  addToSendQueue('0');
  addToSendQueue('\n');
}

void SendLipoVoltage() {
  addToSendQueue('S');
  addToSendQueue(TO_HEX(0));
  addToSendQueue('v');
  uint8_t buf[4];
  float VbatFloat;

  if(getADCVBATmode() != OFF) {
    VbatFloat = (getVbatFloat() / 11.0) * (1024.0 / 5.0); // App expects raw pin reading through a potential divider.
  }

  intToHex(buf, int(VbatFloat));
  addToSendQueue(buf, 4);
  addToSendQueue('\n');
}

void WaitFirstLap(uint8_t NodeAddr) {
  addToSendQueue('S');
  addToSendQueue(TO_HEX(NodeAddr));
  addToSendQueue('1');
  addToSendQueue(TO_HEX(shouldWaitForFirstLap));
  addToSendQueue('\n');
}

void SendTimerCalibration(uint8_t NodeAddr) {

  uint8_t buf[8];
  longToHex(buf, timeAdjustment);
  addToSendQueue('S');
  addToSendQueue(TO_HEX(NodeAddr));
  addToSendQueue('J');
  addToSendQueue(buf, 8);
  addToSendQueue('\n');
}

void SendRaceMode(uint8_t NodeAddr) {

  addToSendQueue('S');
  addToSendQueue(TO_HEX(NodeAddr));
  addToSendQueue('R');
  addToSendQueue(TO_HEX(raceMode));
  addToSendQueue('\n');

}


void SendVRxBand(uint8_t NodeAddr) {
  //Cmd Byte B
  addToSendQueue('S');
  addToSendQueue(TO_HEX(NodeAddr));
  addToSendQueue('B');
  addToSendQueue(TO_HEX(getRXBand(NodeAddr)));
  addToSendQueue('\n');
  //SendVRxFreq(NodeAddr);

}

void SendVRxChannel(uint8_t NodeAddr) {

  addToSendQueue('S');
  addToSendQueue(TO_HEX(NodeAddr));
  addToSendQueue('C');
  addToSendQueue(TO_HEX(getRXChannel(NodeAddr)));
  addToSendQueue('\n');
  //SendVRxFreq(NodeAddr);

}

void SendVRxFreq(uint8_t NodeAddr) {
  //Cmd Byte F
  uint8_t index = getRXChannel(NodeAddr) + (8 * getRXBand(NodeAddr));
  uint16_t frequency = channelFreqTable[index];

  addToSendQueue('S');
  addToSendQueue(TO_HEX(NodeAddr));
  addToSendQueue('F');

  uint8_t buf[4];
  intToHex(buf, frequency);
  addToSendQueue(buf, 4);
  addToSendQueue('\n');
}

void sendAPIversion() {

  for (int i = 0; i < NumRecievers; i++) {
    addToSendQueue('S');
    addToSendQueue(TO_HEX(i));
    addToSendQueue('#');
    addToSendQueue('0');
    addToSendQueue('0');
    addToSendQueue('0');
    addToSendQueue('4');
    addToSendQueue('\n');
  }
}

void SendAllSettings(uint8_t NodeAddr) {
  //1BCFHIJLMRSTvy# + x

  //v Lipo Voltage
  //y Configured State, True/False
  //T Threshold Valule
  //S Enable/Disable Sound
  //R Race Mode
  //M Minimal Lap Time
  //L Lap Report, // laps?
  //J Set Timer Adjustment Value
  //I RSSI monitoring interval
  //H Setup threshold mode
  //F VRx frequqnecy
  //C VRx channel
  //B VRx band
  //1 Enum Devices
  //SendCurrRSSI(NodeAddr);

  //  WaitFirstLap(NodeAddr); //1 Wait First lap
  //  SendVRxBand(NodeAddr); //B
  //  SendVRxChannel(NodeAddr); //C
  //  SendVRxFreq(NodeAddr); //F VRx Freq
  //  SendSetThresholdMode(NodeAddr); //H send Threshold Mode
  //  SendRSSImonitorInterval(NodeAddr); //I RSSI monitor interval
  //  SendTimerCalibration(NodeAddr); //J timer calibration
  //  SendAllLaps(NodeAddr); //L Lap Report
  //  SendMinLap(NodeAddr); //M Minumum Lap Time
  //  SendRaceMode(NodeAddr); //R
  //  SendSoundMode(NodeAddr); //S
  //  SendThresholdValue(NodeAddr); // T
  //  SendLipoVoltage(); // v
  //  SendIsModuleConfigured(NodeAddr); //y
  //  sendAPIversion(); // #
  //  SendXdone(NodeAddr); //x

  SendVRxChannel(NodeAddr);
  SendRaceMode(NodeAddr);
  SendMinLap(NodeAddr);
  SendThresholdValue(NodeAddr);
  SendSoundMode(NodeAddr);
  SendVRxBand(NodeAddr);
  WaitFirstLap(NodeAddr);
  SendIsModuleConfigured();
  SendVRxFreq(NodeAddr);
  SendRSSImonitorInterval(NodeAddr);
  SendTimerCalibration(NodeAddr);
  sendAPIversion();
  sendThresholdMode(NodeAddr);
  SendXdone(NodeAddr);

}

void handleSerialControlInput(char *controlData, uint8_t  ControlByte, uint8_t NodeAddr, uint8_t length) {

  String InString = "";
  uint8_t valueToSet;
  uint8_t NodeAddrByte = TO_BYTE(NodeAddr); // convert ASCII to real byte values

  //Serial.println(length);

  if (ControlByte == CONTROL_NUM_RECIEVERS) {
    CurrNodeAddrAPI = 0;
    CurrNodeAddrLaps = 0;
    SendNumberOfnodes(NodeAddrByte);
  }

  if (controlData[2] == CONTROL_GET_TIME) {
    //Serial.println("Sending Time.....");
    SendMillis();
  }


  if (controlData[2] == 'a') {
    for (int i = 0; i < NumRecievers; i++) {
      SendAllSettings(i);
      //delay(100);
    }
  }

  //  if (controlData[2] == RESPONSE_API_VERSION) {
  //   // for (int i = 0; i < NumRecievers; i++) {
  //      sendAPIversion();
  //    //}
  //  }


  ControlByte = controlData[2]; //This is dirty but we rewrite this byte....

  if (length > 4) { // set value commands  changed to n+1 ie, 3+1 = 4.

    switch (ControlByte) {

      case CONTROL_RACE_MODE:
        valueToSet = TO_BYTE(controlData[3]);
        setRaceMode(valueToSet);
        for (int i = 0; i < NumRecievers; i++) {
          SendRaceMode(i);
        }
        isConfigured = 1;
        break;

      case CONTROL_WAIT_FIRST_LAP:
        valueToSet = TO_BYTE(controlData[3]);
        shouldWaitForFirstLap = valueToSet;
        //playClickTones();
        isConfigured = 1;
        break;

      case CONTROL_BAND:

        setRXBand(NodeAddrByte, TO_BYTE(controlData[3]));
        setModuleChannelBand(NodeAddrByte);
        SendVRxBand(NodeAddrByte);
        SendVRxFreq(NodeAddrByte);
        isConfigured = 1;
        EepromSettings.RXBand[NodeAddrByte] = getRXBand(NodeAddrByte);
        setSaveRequired();
        break;

      case CONTROL_CHANNEL:

        setRXChannel(NodeAddrByte, TO_BYTE(controlData[3]));
        setModuleChannelBand(NodeAddrByte);
        SendVRxChannel(NodeAddrByte);
        SendVRxFreq(NodeAddrByte);
        isConfigured = 1;
        EepromSettings.RXChannel[NodeAddrByte] = getRXChannel(NodeAddrByte);
        setSaveRequired();
        break;

      case CONTROL_FREQUENCY:

        InString += (char)controlData[3];
        InString += (char)controlData[4];
        InString += (char)controlData[5];
        InString += (char)controlData[6];
        RXfrequencies[NodeAddr] = InString.toInt();
        EepromSettings.RXfrequencies[NodeAddr] = RXfrequencies[NodeAddr];
        setSaveRequired();
        setModuleFrequency(RXfrequencies[NodeAddrByte], NodeAddrByte);
        isConfigured = 1;
        break;

      case CONTROL_RSSI_MON_INTERVAL:
        rssiMonitorInterval = (HEX_TO_UINT16((uint8_t*)&controlData[3]));
        lastRssiMonitorReading = 0;
        isConfigured = 1;
        SendRSSImonitorInterval(NodeAddrByte);
        break;

      case CONTROL_MIN_LAP_TIME:
        valueToSet = HEX_TO_BYTE(controlData[3], controlData[4]);
        setMinLap(valueToSet);
        SendMinLap(NodeAddrByte);
        //playClickTones();

        break;

      case CONTROL_SOUND:
        //valueToSet = TO_BYTE(controlData[1]);
        //        isSoundEnabled = valueToSet;
        //        if (!isSoundEnabled) {
        //          noTone(buzzerPin);
        //        }
        //addToSendQueue(SEND_SOUND_STATE);
        //playClickTones();
        for (int i = 0; i < NumRecievers; i++) {
          SendSoundMode(i);
        }
        isConfigured = 1;

        break;

      case CONTROL_THRESHOLD:
        SetThresholdValue(HEX_TO_UINT16((uint8_t*)&controlData[3]), NodeAddrByte);
        SendThresholdValue(NodeAddrByte);
        isConfigured = 1;
        break;
      case CONTROL_TIME_ADJUSTMENT:
        timeAdjustment = HEX_TO_SIGNED_LONG((uint8_t*)&controlData[3]);
        SendTimerCalibration(NodeAddrByte);

        isConfigured = 1;
        break;
      case CONTROL_THRESHOLD_SETUP: // setup threshold using sophisticated algorithm
        valueToSet = TO_BYTE(controlData[3]);
        uint8_t node = TO_BYTE(controlData[1]);
        // Skip this if we get an invalid node id
        if(node >= MaxNumRecievers) {
          break;
        }
        if (!raceMode) { // don't run threshold setup in race mode because we don't calculate slowRssi in race mode, but it's needed for setup threshold algorithm
           thresholdSetupMode[node] = valueToSet;
        }
        if (thresholdSetupMode[node]) {
          setupThreshold(RSSI_SETUP_INITIALIZE, node);
        } else {
          //playThresholdSetupStopTones();
        }
        break;
    }
  } else { // get value and other instructions
    switch (ControlByte) {
      case CONTROL_GET_TIME:
        //millisUponRequest = millis();
        //addToSendQueue(SEND_TIME);
        break;
      case CONTROL_WAIT_FIRST_LAP:
        WaitFirstLap(NodeAddrByte);
        break;
      case CONTROL_BAND:
        SendVRxBand(NodeAddrByte);
        break;
      case CONTROL_CHANNEL:
        SendVRxChannel(NodeAddrByte);
        break;
      case CONTROL_FREQUENCY:
        SendVRxFreq(NodeAddrByte);
        break;
      case CONTROL_RSSI_MON_INTERVAL:
        SendRSSImonitorInterval(NodeAddrByte);
        break;
      case CONTROL_RACE_MODE:
        SendRaceMode(NodeAddrByte);
        break;
      case CONTROL_MIN_LAP_TIME:
        SendMinLap(NodeAddrByte);
        break;
      case CONTROL_SOUND:
        for (int i = 0; i < NumRecievers; i++) {
          SendSoundMode(i);
        }
        break;
      case CONTROL_THRESHOLD:
        SendThresholdValue(NodeAddrByte);
        break;
      case CONTROL_GET_RSSI: // get current RSSI value
        //Serial.println("sending current RSSI");
        for (int i = 0; i < NumRecievers; i++) {
          SendCurrRSSI(i);
        }
        break;
      case CONTROL_GET_VOLTAGE: //get battery voltage
        //addToSendQueue(SEND_VOLTAGE);
        SendLipoVoltage();
        break;
      case CONTROL_GET_ALL_DATA: // request all data
        //addToSendQueue(SEND_ALL_DEVICE_STATE);
        break;
      case CONTROL_GET_API_VERSION: //get API version
        //for (int i = 0; i < NumRecievers; i++) {
        sendAPIversion();
        //}
        break;
      case CONTROL_TIME_ADJUSTMENT:
        SendTimerCalibration(NodeAddrByte);
        break;
      case CONTROL_THRESHOLD_SETUP: // get state of threshold setup process
        sendThresholdMode(NodeAddrByte);
        break;
      case CONTROL_GET_IS_CONFIGURED:
        SendIsModuleConfigured();
        break;
    }
  }
}

void thresholdModeStep() {
  for(uint8_t i = 0; i < MaxNumRecievers; ++i) {
    setupThreshold(RSSI_SETUP_NEXT_STEP, i);
  }
}
