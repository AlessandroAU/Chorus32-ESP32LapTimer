#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__


#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

void bluetooth_update(void* output);
void bluetooth_send_packet(void* output, uint8_t* buf, uint32_t size);
void bluetooth_init(void* output);

#endif // __BLUETOOTH_H__
