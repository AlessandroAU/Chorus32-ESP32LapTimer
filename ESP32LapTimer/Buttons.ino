
#include "Timer.h"

#define buttonTouchThreshold 40
#define buttonDeBounce 200

bool buttonOneTouched = false;
bool buttonTwoTouched = false;

Timer button1Timer = Timer(buttonDeBounce);
Timer button2Timer = Timer(buttonDeBounce);

void IRAM_ATTR buttonOneInterrupt();
void IRAM_ATTR buttonTwoInterrupt();

void buttonSetup() {
  touchAttachInterrupt(BUTTON1, buttonOneInterrupt, buttonTouchThreshold);
  touchAttachInterrupt(BUTTON2, buttonTwoInterrupt, buttonTouchThreshold);
}

void buttonUpdate() {
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
}

void IRAM_ATTR buttonOneInterrupt() {
  buttonOneTouched = true;
}

void IRAM_ATTR buttonTwoInterrupt() {
  buttonTwoTouched = true;
}
