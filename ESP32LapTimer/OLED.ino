#ifdef OLED
// https://techtutorialsx.com/2017/12/02/esp32-arduino-interacting-with-a-ssd1306-oled-display/

#include <Wire.h>
#include "SSD1306.h"

unsigned long OLED_LastUpdated  = 0;
unsigned int OLED_UpdateInterval = 50;

#ifdef AlessandroDevBoard
SSD1306  display(0x3c, 5, 4);
#else
SSD1306  display(0x3c, 21, 22);  // 21 and 22 are default pins
#endif

const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

void oledSetup(void) {

  display.init();
  display.flipScreenVertically();

  //  xTaskCreate(
  //    oledUpdate,          /* Task function. */
  //    "OLED Update",        /* String with name of task. */
  //    50000,            /* Stack size in bytes. */
  //    NULL,             /* Parameter passed as input of the task */
  //    1,                /* Priority of the task. */
  //    NULL);            /* Task handle. */

  delay(100);
}

void OLED_CheckIfUpdateReq() {
  if (millis() > OLED_LastUpdated + OLED_UpdateInterval) {
    oledUpdate();
    OLED_LastUpdated = millis();
  }
  //Serial.println(VbatReadingRaw);
  VbatReadingFloat = fmap(VbatReadingSmooth, 0, 4096, 0, 4.1142);
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


void oledUpdate(void)
{

  //vTaskDelay( xDelay ); //delays 500ms until next update, this is using FreeRTOS functions
  //delay(1000);

  display.clear();

  display.drawLine(0, 11, 127, 11);

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
  display.drawString(127, 0, String(VbatReadingFloat, 3) + "V");

  // Rx modules
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  for (int i = 0; i < NumRecievers; i++) {
    display.drawString(0, 13 + i * 13, String(i + 1) + ") " + getBandLabel(RXBand[i]) + String(RXChannel[i] + 1) + ", " + String(ADCvalues[i] / 12));
  }

  display.display();
}

#endif
