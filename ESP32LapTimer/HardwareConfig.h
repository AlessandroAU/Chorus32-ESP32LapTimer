void InitHardwarePins();

#define VERSION_NUMBER 1 // Increment when eeprom struct modified

#define NumRecievers 4

#define OLED //uncomment this to enable OLED support

///Define Pin configuration here, these are the defaults as given on github

//#define USE_VSPI
#define USE_HSPI

#define SCK 14
#define MOSI 12
#define MISO 13

#define CS1 16
#define CS2 5
#define CS3 4
#define CS4 15

#define ADC1 ADC1_CHANNEL_0
#define ADC2 ADC1_CHANNEL_6
#define ADC3 ADC1_CHANNEL_7
#define ADC4 ADC1_CHANNEL_3
#define ADCVBAT ADC1_CHANNEL_4

#define MIN_TUNE_TIME 30


///////Vbat Monitoring///////
//vbat can either measured over I2S by an INA219 or directly via the ADC. CHOOSE ONE option by uncommenting 

#define VbatINA219 
//#define VbatADC



/////////Custom Hardware options///////////
//#define AlessandroDevBoard //because Alessandro was silly he has to mess around with a different pin configuration 

#ifdef AlessandroDevBoard
  #define OLED
  #define CS2 2
  #define CS3 0
#endif
