#ifdef OLED
// https://techtutorialsx.com/2017/12/02/esp32-arduino-interacting-with-a-ssd1306-oled-display/

#include <Wire.h>
#include "SSD1306Wire.h"

SSD1306Wire  display(0x3c, 21, 22);  // 21 and 22 are default pins

void oledSetup(void) 
{  
  display.init();
  display.flipScreenVertically();
}

void oledUpdate(void) 
{
  display.clear();

  display.drawLine(0, 11, 127, 11);

  // Display on time
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  // Hours
  if (millis()/3600000 < 10) {
    display.drawString(0, 0, "0" + String(millis()/3600000) + ":");  
  } else {
    display.drawString(0, 0, String(millis()/3600000) + ":");  
  }
  // Mins
  if (millis()%3600000/60000 < 10) {
    display.drawString(18, 0, "0" + String(millis()%3600000/60000) + ":");  
  } else {
    display.drawString(18, 0, String(millis()%3600000/60000) + ":");  
  }
  // Seconds
  if (millis()%60000/1000 < 10) {
    display.drawString(36, 0, "0" + String(millis()%60000/1000));  
  } else {
    display.drawString(36, 0, String(millis()%60000/1000));
  }

  // Voltage
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(127, 0, String(VbatReading) + "V");

  // Rx modules
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  for (int i=0; i<NumRecievers; i++) {
    display.drawString(0, 13+i*13, String(i+1) + ") " + getBandLabel(RXBand[i]) + String(RXChannel[i] + 1) + ", " + String(ADCvalues[i]) );    
  }

  display.display();
}

#endif
