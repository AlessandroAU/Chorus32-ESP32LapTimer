#include "OLED.h"

#include <Wire.h>
#include "SSD1306.h"
#include "Font.h"
#include "Timer.h"
#include "Screensaver.h"
#include "ADC.h"
#include "settings_eeprom.h"
#include "RX5808.h"
#include "Calibration.h"

uint8_t oledRefreshTime = 50;

Timer oledTimer = Timer(oledRefreshTime);

SSD1306  display(0x3c, 21, 22);  // 21 and 22 are default pins

const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

uint16_t displayScreenNumber = 0;
uint8_t  numberOfBaseScreens = 4; // Increment if a new screen is added to cycle through.
// The actual number of screens will be calculated on Button 1 press in buttons.ino.
uint8_t numberOfOledScreens = numberOfBaseScreens;

void oledSetup(void) {
  display.init();
  display.flipScreenVertically();
  display.clear();
  display.drawFastImage(0, 0, 128, 64, ChorusLaptimerLogo_Screensaver);
  display.display();
  display.setFont(Dialog_plain_9);
}

void OLED_CheckIfUpdateReq() {
  if (oledTimer.hasTicked()) {
    oledUpdate();
    oledTimer.reset();
  }
}

void displayRxPage() {
  // Gather Data
  uint8_t currentRXNumber = (displayScreenNumber % numberOfOledScreens) - 4;
  uint8_t frequencyIndex = getRXChannel(currentRXNumber) + (8 * getRXBand(currentRXNumber));
  uint16_t frequency = channelFreqTable[frequencyIndex];

  // Display things
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Settings for RX" + String(currentRXNumber + 1));
  display.drawString(0, 18, getBandLabel(getRXBand(currentRXNumber)) + String(getRXChannel(currentRXNumber) + 1) + " - " + frequency);
  if (getRSSI(currentRXNumber) < 600) {
    display.drawProgressBar(48, 35, 120 - 42, 8, map(600, 600, 3500, 0, 85));
  } else {
    display.drawProgressBar(48, 35, 120 - 42, 8, map(getRSSI(currentRXNumber), 600, 3500, 0, 85));
  }
  display.setFont(Dialog_plain_9);
  display.drawString(0,35, "RSSI: " + String(getRSSI(currentRXNumber) / 12));
  display.drawVerticalLine(45 + map(getRSSIThreshold(currentRXNumber), 600, 3500, 0, 85),  35, 8); // line to show the RSSIthresholds
  display.drawString(0,46, "Btn2 SHORT - Channel.");
  display.drawString(0,55, "Btn2 LONG  - Band.");
}

void oledUpdate(void)
{
  display.clear();

  if (displayScreenNumber % numberOfOledScreens == 0) {
    // Display on time
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    // Hours
    if (millis() / 3600000 < 10) {
      display.drawString(0, 0, "0" + String(millis() / 3600000) + ":");
    } else {
      display.drawString(0, 0, String(millis() / 3600000) + ":");
    }
    // Mins
    if (millis() % 3600000 / 60000 < 10) {
      display.drawString(18, 0, "0" + String(millis() % 3600000 / 60000) + ":");
    } else {
      display.drawString(18, 0, String(millis() % 3600000 / 60000) + ":");
    }
    // Seconds
    if (millis() % 60000 / 1000 < 10) {
      display.drawString(36, 0, "0" + String(millis() % 60000 / 1000));
    } else {
      display.drawString(36, 0, String(millis() % 60000 / 1000));
    }

    // Voltage
    if (getADCVBATmode() != 0) {
      display.setTextAlignment(TEXT_ALIGN_RIGHT);
      display.drawString(127, 0, String(getVbatFloat(), 2) + "V");
    }

    if (getADCVBATmode() == INA219) {
      display.drawString(90, 0, String(getMaFloat()/1000, 2) + "A");
    }


    // Rx modules
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    for (int i = 0; i < NumRecievers; i++) {
      display.drawString(0, 9 + i * 9, getBandLabel(getRXBand(i)) + String(getRXChannel(i) + 1) + ", " + String(getRSSI(i) / 12));
      if (getRSSI(i) < 600) {
        display.drawProgressBar(40, 10 + i * 9, 127 - 42, 8, map(600, 600, 3500, 0, 85));
      } else {
        display.drawProgressBar(40, 10 + i * 9, 127 - 42, 8, map(getRSSI(i), 600, 3500, 0, 85));
      }
      display.drawVerticalLine(40 + map(getRSSIThreshold(i), 600, 3500, 0, 85),  10 + i * 9, 8); // line to show the RSSIthresholds
    }
  } else if (displayScreenNumber % numberOfOledScreens == 1) {
    if (getADCVBATmode() != 0) {
      display.setTextAlignment(TEXT_ALIGN_LEFT);

      display.drawString(0, 0, "ADC loop " + String(getADCLoopCount() * (1000.0 / oledRefreshTime)) + " Hz");
      setADCLoopCount(0);

      display.drawString(0, 9, String(getMaFloat()) + " mA");
    }
  } else if (displayScreenNumber % numberOfOledScreens == 2) {
    if (getADCVBATmode() != 0) {
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.drawString(0, 0, "Frequency - " + String(channelFreqTable[getcalibrationFreqIndex()]) + "Hz");
      display.drawString(0,  9, "Min = " + String(EepromSettings.RxCalibrationMin[0]) + ", Max = " + String(EepromSettings.RxCalibrationMax[0]));
      display.drawString(0, 18, "Min = " + String(EepromSettings.RxCalibrationMin[1]) + ", Max = " + String(EepromSettings.RxCalibrationMax[1]));
      display.drawString(0, 27, "Min = " + String(EepromSettings.RxCalibrationMin[2]) + ", Max = " + String(EepromSettings.RxCalibrationMax[2]));
      display.drawString(0, 36, "Min = " + String(EepromSettings.RxCalibrationMin[3]) + ", Max = " + String(EepromSettings.RxCalibrationMax[3]));
      display.drawString(0, 45, "Min = " + String(EepromSettings.RxCalibrationMin[4]) + ", Max = " + String(EepromSettings.RxCalibrationMax[4]));
      display.drawString(0, 54, "Min = " + String(EepromSettings.RxCalibrationMin[5]) + ", Max = " + String(EepromSettings.RxCalibrationMax[5]));
    }
  } else if (displayScreenNumber % numberOfOledScreens == 3) {
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, "Airplane Mode Settings:");
    display.drawString(0, 15, "Long Press Button 2 to");
    display.drawString(0, 26, "toggle Airplane mode.");
    // TODO: implement airplane mode
    //if (!airplaneMode) {
    if (false) {
      display.drawString(0, 42, "Airplane Mode: OFF");
      display.drawString(0, 51, "WiFi: ON  | Draw: " + String(getMaFloat()/1000, 2) + "A");
    } else {
      display.drawString(0, 42, "Airplane Mode: ON");
      display.drawString(0, 51, "WiFi: OFF  | Draw: " + String(getMaFloat()/1000, 2) + "A");
    }
  } else if (displayScreenNumber % numberOfOledScreens >= 4 && displayScreenNumber % numberOfOledScreens <= 9) {
    // RX Settings Pages here.
    displayRxPage();
  }

  display.display();
}

void incrementRxFrequency() {
  uint8_t currentRXNumber = (displayScreenNumber % numberOfOledScreens) - 4;
  uint8_t currentRXChannel = getRXChannel(currentRXNumber);
  uint8_t currentRXBand = getRXBand(currentRXNumber);
  currentRXChannel++;
  if (currentRXChannel >= 8) {
    //currentRXBand++;
    currentRXChannel = 0;
  }
  if (currentRXBand >= 7 && currentRXChannel >= 2) {
    currentRXBand = 0;
    currentRXChannel = 0;
  }
  
  setModuleChannelBand(currentRXChannel,currentRXBand,currentRXNumber); 
}
void incrementRxBand() {
  uint8_t currentRXNumber = (displayScreenNumber % numberOfOledScreens) - 4;
  uint8_t currentRXChannel = getRXChannel(currentRXNumber);
  uint8_t currentRXBand = getRXBand(currentRXNumber);
  currentRXBand++;
  if (currentRXBand >= 8) {
    currentRXBand = 0;
  }
  setModuleChannelBand(currentRXChannel,currentRXBand,currentRXNumber); 
}

void setDisplayScreenNumber(uint16_t num) {
  displayScreenNumber = num;
}

uint16_t getDisplayScreenNumber() {
  return displayScreenNumber;
}

void setNumberOfOledScreens(uint8_t num) {
  numberOfOledScreens = num;
}
uint8_t getNumberOfOledScreens() {
  return numberOfOledScreens;
}

void setNumberOfBaseScreens(uint8_t num) {
  numberOfBaseScreens = num;
}

uint8_t getNumberOfBaseScreens(){
  return numberOfBaseScreens;
}
