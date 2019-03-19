#include "TCP.h"
#include "UDP.h"

void IRAM_ATTR addToSendQueue(uint8_t item) {
  UDPoutQue[UDPoutQuePtr] = item;
  if (UDPoutQuePtr < UDPbuffSize - 1) {
    UDPoutQuePtr = UDPoutQuePtr + 1;
  } else {
    UDPoutQuePtr = 0;
  }
  //  Serial.println(UDPoutQuePtr);
  //UDPoutQuePtr++;


  TCPoutQue[TCPoutQuePtr] = item;
  if (TCPoutQuePtr < TCPbuffSize - 1) {
    TCPoutQuePtr = TCPoutQuePtr + 1;
  } else {
    TCPoutQuePtr = 0;
  }

#ifdef BluetoothEnabled
  BluetoothBuffOut[BluetoothBuffOutPointer] = item;
  BluetoothBuffOutPointer++;
#endif

  //Serial.print((char)item);

  // if (item == '\n') {
  //   SendUDPpacket();
  // }
}


void IRAM_ATTR addToSendQueue(uint8_t * buff, uint8_t length) {

  for (int i = 0; i < length; i++) {
    UDPoutQue[UDPoutQuePtr] = buff[i];
    if (UDPoutQuePtr < UDPbuffSize - 1) {
      UDPoutQuePtr = UDPoutQuePtr + 1;
    } else {
      UDPoutQuePtr = 0;
    }
  }

  for (int i = 0; i < length; i++) {
    TCPoutQue[TCPoutQuePtr] = buff[i];
    if (TCPoutQuePtr < TCPbuffSize - 1) {
      TCPoutQuePtr = TCPoutQuePtr + 1;
    } else {
      TCPoutQuePtr = 0;
    }
  }

  //  for (int i = 0; i < length; i++) {
  //    TCPoutQue[TCPoutQuePtr] = buff[i];
  //    TCPoutQuePtr++;
  //  }

  //  for (int i = 0; i < length; i++) {
  //    Serial.print((char)buff[i]);
  //
  //  }

#ifdef BluetoothEnabled
  BluetoothBuffOut[BluetoothBuffOutPointer] = buff[i];
  BluetoothBuffOutPointer++;
#endif

  //    if (MirrorToSerial) {
  //      Serial.print(char(buff[i]));
  //    }

}
