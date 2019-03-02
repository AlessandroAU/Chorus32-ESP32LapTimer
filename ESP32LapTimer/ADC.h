#include "HardwareConfig.h"

#define ADCmemLen 150

uint32_t SampleArrayMillisOffset; //value that stores the offset of the global millis(); time so we can relate the array sample time to the the global time via this offset. // Not used for the moment.

uint16_t ADC1readings[ADCmemLen];
uint16_t ADC2readings[ADCmemLen];
uint16_t ADC3readings[ADCmemLen];
uint16_t ADC4readings[ADCmemLen];
uint16_t ADCVBATreadings[ADCmemLen];

uint16_t ADC1readingsRAW[ADCmemLen];
uint16_t ADC2readingsRAW[ADCmemLen];
uint16_t ADC3readingsRAW[ADCmemLen];
uint16_t ADC4readingsRAW[ADCmemLen];
uint16_t ADCVBATreadingsRAW[ADCmemLen];

int RSSIthresholds[NumRecievers];

uint16_t ADC1ReadingRAW;
uint16_t ADC2ReadingRAW;
uint16_t ADC3ReadingRAW;
uint16_t ADC4ReadingRAW;
uint16_t ADC5ReadingRAW;
uint16_t ADC6ReadingRAW;
uint16_t ADCVBATreadingRAW;


//uint16_t ADC1ptr = 0; //not really pointer just keeps track of which sample we are up too
//uint16_t ADC2ptr = 0; //same as above
//uint16_t ADC3ptr = 0;
//uint16_t ADC4ptr = 0;
//uint16_t ADCVBATptr = 0;

int ADC1value;
int ADC2value;
int ADC3value;
int ADC4value;

float VbatReadingFloat;

unsigned int VbatReadingRaw;
unsigned int VbatReadingSmooth;

int FilteredADCvalues[6];
int ADCvalues[6];

void ConfigureADC();
void InitADCtimer();
void IRAM_ATTR CheckRSSIthresholdExceeded();
