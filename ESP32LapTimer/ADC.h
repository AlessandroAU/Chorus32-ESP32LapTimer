#pragma once

#include "HardwareConfig.h"
#include "Filter.h"

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

int RSSIthresholds[MaxNumRecievers];

uint16_t ADCReadingsRAW[MaxNumRecievers];

uint16_t ADC1ReadingRAW;
uint16_t ADC2ReadingRAW;
uint16_t ADC3ReadingRAW;
uint16_t ADC4ReadingRAW;
uint16_t ADC5ReadingRAW;
uint16_t ADC6ReadingRAW;
uint16_t ADCVBATreadingRAW;

int ADC1value;
int ADC2value;
int ADC3value;
int ADC4value;

float VBATcalibration;

float VbatReadingFloat;
float mAReadingFloat;

unsigned int VbatReadingRaw;
unsigned int VbatReadingSmooth;

int FilteredADCvalues[MaxNumRecievers];
int ADCvalues[MaxNumRecievers];

void ConfigureADC();
void InitADCtimer();
void IRAM_ATTR CheckRSSIthresholdExceeded();
uint16_t adcLoopCounter = 0;

FilterBeLp2_10HZ Filter_10HZ[6] = {FilterBeLp2_10HZ(), FilterBeLp2_10HZ(), FilterBeLp2_10HZ(), FilterBeLp2_10HZ(), FilterBeLp2_10HZ(), FilterBeLp2_10HZ()};
FilterBeLp2_20HZ Filter_20HZ[6] = {FilterBeLp2_20HZ(), FilterBeLp2_20HZ(), FilterBeLp2_20HZ(), FilterBeLp2_20HZ(), FilterBeLp2_20HZ(), FilterBeLp2_20HZ()};
FilterBeLp2_50HZ Filter_50HZ[6] = {FilterBeLp2_50HZ(), FilterBeLp2_50HZ(), FilterBeLp2_50HZ(), FilterBeLp2_50HZ(), FilterBeLp2_50HZ(), FilterBeLp2_50HZ()};
FilterBeLp2_100HZ Filter_100HZ[6] = {FilterBeLp2_100HZ(), FilterBeLp2_100HZ(), FilterBeLp2_100HZ(), FilterBeLp2_100HZ(), FilterBeLp2_100HZ(), FilterBeLp2_100HZ()};
