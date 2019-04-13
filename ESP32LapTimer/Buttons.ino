
#include "Timer.h"

#define buttonTouchThreshold 40
#define buttonDeBounce 200

#define newButtonDeBounce 40
// This is needed otherwise the screens will not initialize properly on startup
uint8_t numberOfOledScreens = numberOfBaseScreens;

void IRAM_ATTR buttonOneInterrupt();
void IRAM_ATTR buttonTwoInterrupt();

bool buttonOneTouched = false;
bool buttonTwoTouched = false;

Timer button1Timer = Timer(buttonDeBounce);
Timer button2Timer = Timer(buttonDeBounce);

long buttonLongPressTime = 800;
long buttonTimer1 = 0;
long buttonTimer2 = 0;
long touchedTime1 = 0;
long touchedTime2 = 0;

byte touch1;
byte touch2;

bool buttonActive1 = false;
bool longPressActive1 = false;
bool buttonPressed1 = false;
bool buttonActive2 = false;
bool longPressActive2 = false;
bool buttonPressed2 = false;

void newButtonSetup() {
  touch_pad_filter_start(BUTTON1);
  touch_pad_set_filter_period(BUTTON1);
  touch_pad_filter_start(BUTTON2);
  touch_pad_set_filter_period(BUTTON2);
}

void buttonSetup() {
  touchAttachInterrupt(BUTTON1, buttonOneInterrupt, buttonTouchThreshold);
  touchAttachInterrupt(BUTTON2, buttonTwoInterrupt, buttonTouchThreshold);
}

void touchMonitor() {
  byte touch = touchRead(BUTTON1);
  Serial.println(touch);
  Serial.println(longPressActive1);
  Serial.println(longPressActive2);
  delay(100);
}

void newButtonUpdate() {
  touch1 = touchRead(BUTTON1); // Read the state of button 1
  touch2 = touchRead(BUTTON2); // Read the state of button 2

  // BUTTON 1 Debounce logic here. Basically, we only read a button touch if
  // it stays below threshol for newButtonDebounce, it gets flagged as "pressed". 
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
      if (longPressActive2) {
      }
      // vvv BUTTON 1 LONG press between these comments vvv
      
      doubleBeep();
      displayScreenNumber = 0;

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
        
        beep();
        numberOfOledScreens = numberOfBaseScreens + (NumRecievers); // Re-calculating the number of screens while cycling through them
        displayScreenNumber++;

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

      doubleBeep();
       if (displayScreenNumber % numberOfOledScreens >= 4 && displayScreenNumber % numberOfOledScreens <= 9) {
          // Increment RX Frequency Here.
          incrementRxBand();
        }

        if (displayScreenNumber % numberOfOledScreens == 3) {
          // Toggle Airplane Mode
          toggleAirplaneMode();
        }

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

        beep();
        if (displayScreenNumber % numberOfOledScreens == 2) {
          rssiCalibration();
        }
        if (displayScreenNumber % numberOfOledScreens >= 4 && displayScreenNumber % numberOfOledScreens <= 9) {
          // Increment RX Frequency Here.
          incrementRxFrequency();
        }
        
        // ^^^ BUTTON 2 SHORT press between these comments ^^^
      }
      buttonActive2 = false;
    }
  }

  if (longPressActive1 && longPressActive2) {
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

void buttonUpdate() {
  if(buttonOneTouched && button1Timer.hasTicked()) {
    Serial.println("buttonOneTouched");
    beep();
    
    // Do button1 stuff in here
    numberOfOledScreens = numberOfBaseScreens + (NumRecievers); // Re-calculating the number of screens while cycling through them
    displayScreenNumber++;
    
    buttonOneTouched = false;
    button1Timer.reset();
  } else {
    buttonOneTouched = false;    
  }
  
  if(buttonTwoTouched &&  button2Timer.hasTicked()) {
    Serial.println("buttonTwoTouched");
    beep();
    
    // Do button2 stuff in here
    if (displayScreenNumber % numberOfOledScreens == 2) {
      rssiCalibration();
    }
    
    if (displayScreenNumber % numberOfOledScreens == 3) {
      // Toggle Airplane Mode
      toggleAirplaneMode();
    }

    if (displayScreenNumber % numberOfOledScreens >= 4 && displayScreenNumber % numberOfOledScreens <= 9) {
      // Increment RX Frequency Here.
      incrementRxFrequency();
    }
  
    buttonTwoTouched = false;
    button2Timer.reset();
  } else {
    buttonTwoTouched = false;    
  }
}

void IRAM_ATTR buttonOneInterrupt() {
  buttonOneTouched = true;
}

void IRAM_ATTR buttonTwoInterrupt() {
  buttonTwoTouched = true;
}
