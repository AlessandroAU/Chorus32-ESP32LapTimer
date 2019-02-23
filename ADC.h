#include "HardwareConfig.h"

#define ADCmemLen 100

uint32_t SampleArrayMillisOffset; //value that stores the offset of the global millis(); time so we can relate the array sample time to the the global time via this offset. // Not used for the moment.

uint16_t ADC1readings[ADCmemLen];
uint16_t ADC2readings[ADCmemLen];
uint16_t ADC3readings[ADCmemLen];
uint16_t ADC4readings[ADCmemLen];

uint16_t ADC1readingsRAW[ADCmemLen];
uint16_t ADC2readingsRAW[ADCmemLen];
uint16_t ADC3readingsRAW[ADCmemLen];
uint16_t ADC4readingsRAW[ADCmemLen];


uint16_t ADC1ptr = 0; //not really pointer just keeps track of which sample we are up too
uint16_t ADC2ptr = 0; //same as above
uint16_t ADC3ptr = 0;
uint16_t ADC4ptr = 0;

int ADC1value;
int ADC2value;
int ADC3value;
int ADC4value;

float VbatReading;

int FilteredADCvalues[NumRecievers];
int ADCvalues[NumRecievers];

void ConfigureADC();
void InitADCtimer();
