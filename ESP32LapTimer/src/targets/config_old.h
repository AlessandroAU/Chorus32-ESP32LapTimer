#define BEEPER 23

#define LEDs 17

#define BUTTON1 T7 // 27
#define BUTTON2 T4 // 13

#define VRX_SCK 14
#define VRX_MOSI 12

// Ordering of pins is important.
// E.g the first cs pin should match the first adc pin.
#define CS_PINS 16, 5, 4, 15, 25, 26
#define ADC_PINS ADC1_CHANNEL_0, \
                 ADC1_CHANNEL_6, \
                 ADC1_CHANNEL_7, \
                 ADC1_CHANNEL_3, \
                 ADC1_CHANNEL_4, \
                 ADC1_CHANNEL_5

#define ADC1_GPIO 36
#define ADC2_GPIO 34
#define ADC3_GPIO 35
#define ADC4_GPIO 39
#define ADC5_GPIO 32
#define ADC6_GPIO 33
