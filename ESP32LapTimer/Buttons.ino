
#define buttonTouchThreshold 40
#define buttonDeBounce 200

bool buttonOneTouched = false;
bool buttonTwoTouched = false;

long buttonOneLastTouchedTime = 0;
long buttonTwoLastTouchedTime = 0;

void buttonSetup() {
  touchAttachInterrupt(T4, buttonOneInterrupt, buttonTouchThreshold);
  touchAttachInterrupt(T7, buttonTwoInterrupt, buttonTouchThreshold);
}

void buttonUpdate() {
  if(buttonOneTouched && millis() > (buttonOneLastTouchedTime + buttonDeBounce)) {
    Serial.println("buttonOneTouched");
    // Do button1 stuff in here
    buttonOneTouched = false;
    buttonOneLastTouchedTime = millis();
  } else {
    buttonOneTouched = false;    
  }
  
  if(buttonTwoTouched && millis() > (buttonTwoLastTouchedTime + buttonDeBounce)) {
    Serial.println("buttonTwoTouched");
    // Do button2 stuff in here
    buttonTwoTouched = false;
    buttonTwoLastTouchedTime = millis();
  } else {
    buttonTwoTouched = false;    
  }
}

void buttonOneInterrupt() {
  buttonOneTouched = true;
}

void buttonTwoInterrupt() {
  buttonTwoTouched = true;
}
