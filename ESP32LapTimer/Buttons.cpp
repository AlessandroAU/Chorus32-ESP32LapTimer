#include "Buttons.h"

#include "Timer.h"
#include "HardwareConfig.h"
#include "Beeper.h"
#include "OLED.h"
#include "Calibration.h"
#include "settings_eeprom.h"
#include "WebServer.h"

#include <stdint.h>

#define newButtonDeBounce 40

static bool buttonOneTouched = false;
static bool buttonTwoTouched = false;

static long buttonLongPressTime = 800; // How long to hold before a longtouch is registered

// Timers to keep track of when the buttons were pressed
static long buttonTimer1 = 0;
static long buttonTimer2 = 0;
static long touchedTime1 = 0;
static long touchedTime2 = 0;

// These are for the value of the capacitive touch. 
static uint8_t touch1;
static uint8_t touch2;

// Bools to help with debounce and long touch
static bool buttonActive1 = false;
static bool longPressActive1 = false;
static bool buttonPressed1 = false;
static bool buttonActive2 = false;
static bool longPressActive2 = false;
static bool buttonPressed2 = false;

void newButtonSetup() {
#ifdef USE_NORMAL_BUTTONS
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
#else
  touch_pad_filter_start(BUTTON1);
  touch_pad_set_filter_period(BUTTON1);
  touch_pad_filter_start(BUTTON2);
  touch_pad_set_filter_period(BUTTON2);
#endif
}

void newButtonUpdate() {
#ifdef USE_NORMAL_BUTTONS
  touch1 = (digitalRead(BUTTON1)) ? 100 : 0;
  touch2 = (digitalRead(BUTTON2)) ? 100 : 0;
#else
  touch1 = touchRead(BUTTON1); // Read the state of button 1
  touch2 = touchRead(BUTTON2); // Read the state of button 2
#endif

  // BUTTON 1 Debounce logic here. Basically, we only read a button touch if
  // it stays below threshold for newButtonDebounce, it gets flagged as "pressed". 
  if (buttonOneTouched == false && touch1 < 70) {
    touchedTime1 = millis();
    buttonOneTouched = true;
  } else if (buttonOneTouched == true && touch1 < 70) {
    if ( millis() - touchedTime1 >= newButtonDeBounce) {
      buttonPressed1 = true;
    }
  } else if (touch1 > 50) {
    buttonOneTouched = false;
    buttonPressed1 = false;
  }

  // BUTTON 2 Debounce logic here. Basically, we only read a button touch if
  // it stays below threshol for newButtonDebounce, it gets flagged as "pressed". 
  if (buttonTwoTouched == false && touch2 < 70) {
    touchedTime2 = millis();
    buttonTwoTouched = true;
  } else if (buttonTwoTouched == true && touch2 < 70) {
    if ( millis() - touchedTime2 >= newButtonDeBounce) {
      buttonPressed2 = true;
    }
  } else if (touch2 > 50) {
    buttonTwoTouched = false;
    buttonPressed2 = false;
  }

  // BUTTON 1 Logic Here
  if (buttonPressed1) {
    if (buttonActive1 == false) {
      buttonActive1 = true;
      buttonTimer1 = millis();
    }
    if ((millis() - buttonTimer1 > buttonLongPressTime) && (longPressActive1 == false)) {
      Serial.println("Button 1 Long Press.");
      // vvv BUTTON 1 LONG press between these comments vvv
      oledInjectInput(0, BUTTON_LONG);
      // ^^^ BUTTON 1 LONG press between these comments ^^^
      longPressActive1 = true;
    }
  } else {
    if (buttonActive1 == true) {
      if (longPressActive1 == true) {
        longPressActive1 = false;
      } else {
        Serial.println("Button 1 Short press.");
        // vvv BUTTON 1 SHORT press between these comments vvv
        oledInjectInput(0, BUTTON_SHORT);
        // ^^^ BUTTON 1 SHORT press between these comments ^^^
      }
      buttonActive1 = false;
    }
  }

  // BUTTON 2 Logic here
  if (buttonPressed2) {
    if (buttonActive2 == false) {
      buttonActive2 = true;
      buttonTimer2 = millis();
    }
    if ((millis() - buttonTimer2 > buttonLongPressTime) && (longPressActive2 == false)) {
      Serial.println("Button 2 Long Press.");
      // vvv BUTTON 2 LONG press between these comments vvv

     oledInjectInput(1, BUTTON_LONG);

      // ^^^ BUTTON 2 LONG press between these comments ^^^
      longPressActive2 = true;
    }
  } else {
    if (buttonActive2 == true) {
      if (longPressActive2 == true) {
        longPressActive2 = false;
      } else {
        Serial.println("Button 2 Short press.");
        // vvv BUTTON 2 SHORT press between these comments vvv
        oledInjectInput(1, BUTTON_SHORT);
        // ^^^ BUTTON 2 SHORT press between these comments ^^^
      }
      buttonActive2 = false;
    }
  }

  if (longPressActive1 && longPressActive2) {
    // Long press on both buttons gets you here
    delay(200);
    chirps();
    delay(1000);
    Serial.println("RESET");
    fiveBeep();
    EepromSettings.defaults();
    delay(100);
    ESP.restart();
  }
}
