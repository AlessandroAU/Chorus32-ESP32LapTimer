#ifndef __OLED_H__
#define __OLED_H__

#include <stdint.h>

void oledSetup();
void oledUpdate();
void OLED_CheckIfUpdateReq();
void setDisplayScreenNumber(uint16_t num);


#endif // __OLED_H__
