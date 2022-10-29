#define LEDs 0

#define VRX_SCK 18
#define VRX_MOSI 23


// #define USE_VSPI

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

#define STATUS_LED 2 // schematic says 15 but this is wrong in the pcb.

#define BUTTON1 -1 
#define BUTTON2 -1 

#ifdef OLED
#undef OLED
#endif

// enable arduino ota by default, since there is no USB connector on this board
#ifndef USE_ARDUINO_OTA
#define USE_ARDUINO_OTA
#endif
