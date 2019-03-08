#ifdef OLED
// https://techtutorialsx.com/2017/12/02/esp32-arduino-interacting-with-a-ssd1306-oled-display/

#include <Wire.h>
#include "SSD1306.h"
#include "Font.h"
#include "Timer.h"
#include "Screensaver.h"

Timer oledTimer = Timer(50);

#ifdef AlessandroDevBoard
SSD1306  display(0x3c, 5, 4);
#else
SSD1306  display(0x3c, 21, 22);  // 21 and 22 are default pins
#endif

const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

void oledSetup(void) {

  display.init();
  display.flipScreenVertically();
  display.clear();
  display.drawFastImage(0, 0, 128, 64, ChorusLaptimerLogo_Screensaver);
  display.display();
  display.setFont(Dialog_plain_9);

  //  xTaskCreate(
  //    oledUpdateTask,          /* Task function. */
  //    "oledUpdateTask",        /* String with name of task. */
  //    10000,            /* Stack size in bytes. */
  //    NULL,             /* Parameter passed as input of the task */
  //    10,                /* Priority of the task. */
  //    NULL);            /* Task handle. */

  //delay(100);
}

void OLED_CheckIfUpdateReq() {
  if (oledTimer.hasTicked()) {
    oledUpdate();
    oledTimer.reset();
  }
  //Serial.println(VbatReadingRaw);

  if (ADCVBATmode != INA219) {
    VbatReadingFloat = fmap(VbatReadingSmooth * VBATcalibration, 0, 4096, 0, 4.4);
  }

  //Serial.println(VbatReading);

}

//
//void oledUpdate()
//{
//
//  delay(1000);
//  display.clear();
//
//  display.setColor(WHITE);
//  display.setTextAlignment(TEXT_ALIGN_CENTER);
//  display.drawString(64, 15, String(10));
//  display.setFont(ArialMT_Plain_24);
//
//  display.display();
//
//  delay(10);
//}

void oledUpdateTask( void * parameter ) {
  while (1) {
    oledUpdate();
    vTaskDelay(500);
  }
}


void oledUpdate(void)
{

  //vTaskDelay( xDelay ); //delays 500ms until next update, this is using FreeRTOS functions
  //delay(1000);

  display.clear();

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
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(127, 0, String(VbatReadingFloat, 2) + "V");

  // Rx modules
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  for (int i = 0; i < NumRecievers; i++) {
    display.drawString(0, 9 + i * 9, getBandLabel(RXBand[i]) + String(RXChannel[i] + 1) + ", " + String(ADCvalues[i] / 12));
    display.drawProgressBar(40, 10 + i * 9, 127 - 42, 8, map(ADCvalues[i], 100, 4096, 0, 85));
  }

  display.display();
}

#endif
