#pragma once
void InitHardwarePins();

#define EEPROM_VERSION_NUMBER 5 // Increment when eeprom struct modified

#define MaxNumRecievers 6
byte NumRecievers;

#define OLED //uncomment this to enable OLED support

///Define Pin configuration here, these are the defaults as given on github

#define BEEPER 26

#define USE_VSPI
//#define USE_HSPI

#define SCK 14
#define MOSI 12
#define MISO 13

#define CS1 16
#define CS2 5
#define CS3 4
#define CS4 15
#define CS5 25
#define CS6 26

#define ADC1 ADC1_CHANNEL_0
#define ADC2 ADC1_CHANNEL_6
#define ADC3 ADC1_CHANNEL_7
#define ADC4 ADC1_CHANNEL_3
#define ADC5 ADC1_CHANNEL_4
#define ADC6 ADC1_CHANNEL_5

#define ADC1_GPIO 36
#define ADC2_GPIO 34
#define ADC3_GPIO 35
#define ADC4_GPIO 39
#define ADC5_GPIO 32
#define ADC6_GPIO 33


#define MIN_TUNE_TIME 30


///////Vbat Monitoring///////
//vbat can either measured over I2S by an INA219 or directly via the ADC. Use the bult in web interface at 192.168.4.1 to select


/////////Custom Hardware options///////////
//#define AlessandroDevBoard //because Alessandro was silly he has to mess around with a different pin configuration 

#ifdef AlessandroDevBoard
#define OLED
#define CS2 2
#define CS3 0
#endif
