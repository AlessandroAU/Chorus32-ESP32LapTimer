#define BEEPER 14

#define LEDs 2

#define USE_CLASSIC_BUTTONS

#define BUTTON1 13
#define BUTTON2 12

// Ordering of pins is important.
// E.g the first cs pin should match the first adc pin.
#define CS_PINS 16, 5, 4, 15, 25, 26
#define ADC_PINS ADC1_CHANNEL_0, \
                 ADC1_CHANNEL_3, \
                 ADC1_CHANNEL_6, \
                 ADC1_CHANNEL_5, \
                 ADC1_CHANNEL_4, \
                 ADC1_CHANNEL_7 // switched with adc 4 due to xtal

#define ADC1_GPIO 36
#define ADC2_GPIO 39
#define ADC3_GPIO 34
#define ADC4_GPIO 33
#define ADC5_GPIO 32
#define ADC6_GPIO 35
