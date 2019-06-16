#include "Bluetooth.h"

#ifdef BluetoothEnabled

char BluetootBuffIn[255];
int BluetootBuffInPointer = 0;

char BluetootBufftoProcess[255];

char BluetoothBuffOut[255];
int BluetoothBuffOutPointer=0;

BluetoothSerial SerialBT;

void ReadfromBLE() {

  if (SerialBT.available()) {
    BluetootBuffIn[BluetootBuffInPointer] = SerialBT.read();
    Serial.print(BluetootBuffIn[BluetootBuffInPointer]);

    if (BluetootBuffIn[BluetootBuffInPointer] == '\n') {
      //      for (int i  = 0 ; i < BluetoothBuffOutPointer; i++) {
      //        Serial.print(BluetootBufftoProcess[i]);
      //      }
      memcpy(BluetootBufftoProcess, BluetootBuffIn, BluetootBuffInPointer);
      uint8_t ControlPacket = BluetootBufftoProcess[0];
      uint8_t NodeAddr = BluetootBufftoProcess[1];
      handleSerialControlInput(BluetootBufftoProcess, ControlPacket, NodeAddr, BluetootBuffInPointer);
      BluetootBuffInPointer = 0;
    } else {
      BluetootBuffInPointer++;
    }
  }
}

void SendtoBLE() {
  if (SerialBT.hasClient()) {
    if (BluetoothBuffOut[BluetoothBuffOutPointer - 1] == '\n') {
      SerialBT.write((const uint8_t*)BluetoothBuffOut, BluetoothBuffOutPointer);
      BluetoothBuffOutPointer = 0;
    }
  }
}
void HandleBluetooth() {
  SendtoBLE();
  ReadfromBLE();
}

#endif 
