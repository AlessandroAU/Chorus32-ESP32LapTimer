#pragma once

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

char BluetootBuffIn[255];
int BluetootBuffInPointer = 0;

char BluetootBufftoProcess[255];

char BluetoothBuffOut[255];
int BluetoothBuffOutPointer=0;

BluetoothSerial SerialBT;
