
#include "Timer.h"

#define buttonTouchThreshold 40
#define buttonDeBounce 200

#ifndef Micro32-PCB
bool buttonOneTouched = false;
bool buttonTwoTouched = false;

Timer button1Timer = Timer(buttonDeBounce);
Timer button2Timer = Timer(buttonDeBounce);

void IRAM_ATTR buttonOneInterrupt();
void IRAM_ATTR buttonTwoInterrupt();
#endif

void buttonSetup() {
  #ifndef Micro32-PCB
  touchAttachInterrupt(BUTTON1, buttonOneInterrupt, buttonTouchThreshold);
  touchAttachInterrupt(BUTTON2, buttonTwoInterrupt, buttonTouchThreshold);
  #endif
}

void buttonUpdate() {
  #ifndef Micro32-PCB
  if(buttonOneTouched && button1Timer.hasTicked()) {
    Serial.println("buttonOneTouched");
    beep();
    
    // Do button1 stuff in here
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
    buttonTwoTouched = false;
    button2Timer.reset();
  } else {
    buttonTwoTouched = false;    
  }
  #endif
}

#ifndef Micro32-PCB
void IRAM_ATTR buttonOneInterrupt() {
  buttonOneTouched = true;
}

void IRAM_ATTR buttonTwoInterrupt() {
  buttonTwoTouched = true;
}
#endif
