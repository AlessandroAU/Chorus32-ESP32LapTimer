#pragma once
void InitHardwarePins();

///Define Pin configuration here, these are the defaults as given on github

#define Micro32-PCB
//#define Default_Pins        // Use for all new hardware and PCBs
//#define Old_Default_Pins


// DO NOT CHANGE BELOW UNLESS USING CUSTOM HARDWARE

#define EEPROM_VERSION_NUMBER 6 // Increment when eeprom struct modified

#define MaxNumRecievers 6
byte NumRecievers;

#define MIN_TUNE_TIME 30000 // value in micro seconds

#define OLED //uncomment this to enable OLED support

#ifdef Default_Pins
  #define BEEPER 14
  
  #define LEDs 12

  #define BUTTON1 T7 // 27
  #define BUTTON2 T4 // 13
  
  #define USE_VSPI
  
  #define CS1 16
  #define CS2 5
  #define CS3 4
  #define CS4 15
  #define CS5 25
  #define CS6 26
  
  #define ADC1 ADC1_CHANNEL_0
  #define ADC2 ADC1_CHANNEL_3
  #define ADC3 ADC1_CHANNEL_6
  #define ADC4 ADC1_CHANNEL_7
  #define ADC5 ADC1_CHANNEL_4
  #define ADC6 ADC1_CHANNEL_5
  
  #define ADC1_GPIO 36
  #define ADC2_GPIO 39
  #define ADC3_GPIO 34
  #define ADC4_GPIO 35
  #define ADC5_GPIO 32
  #define ADC6_GPIO 33
#endif

#ifdef Micro32-PCB
  #define USE_VSPI
  
  #define CS1 27
  #define CS2 26
  #define CS3 9
  #define CS4 13
  #define CS5 14
  #define CS6 12

  #define ADC1 ADC1_CHANNEL_0
  #define ADC2 ADC1_CHANNEL_3
  #define ADC3 ADC1_CHANNEL_7
  #define ADC4 ADC1_CHANNEL_6
  #define ADC5 ADC1_CHANNEL_4
  #define ADC6 ADC1_CHANNEL_5
  
  #define ADC1_GPIO 36
  #define ADC2_GPIO 39
  #define ADC3_GPIO 35
  #define ADC4_GPIO 34
  #define ADC5_GPIO 32
  #define ADC6_GPIO 33

  #define BEEPER 10
  #define ADC_SELECT 25
  #define LED_BLUE 2
  #define LED_RED 4
  #define LED_GREEN 19

  #define STATUS_LED 15

#endif

#ifdef Old_Default_Pins
  #define BEEPER 23
  
  #define LEDs 17

  #define BUTTON1 T7 // 27
  #define BUTTON2 T4 // 13
  
  #define USE_HSPI
  
  #define SCK 14
  #define MOSI 12
  
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
#endif
