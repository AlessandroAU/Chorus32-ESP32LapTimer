void IRAM_ATTR SendUDPpacket() {

  if (UDPoutQuePtr > 0) {
    IPAddress remoteIp = UDPserver.remoteIP();
    uint16_t port = UDPserver.remotePort();
    UDPserver.beginPacket(remoteIp, port);
    UDPserver.write((const uint8_t *)UDPoutQue, UDPoutQuePtr);
    UDPserver.endPacket();
    UDPoutQuePtr = 0;
  }
}


void IRAM_ATTR addToSendQueue(uint8_t item) {
  UDPoutQue[UDPoutQuePtr] = item;
  UDPoutQuePtr++;

  BluetoothBuffOut[BluetoothBuffOutPointer] = item;
  BluetoothBuffOutPointer++;

  if (MirrorToSerial) {
    Serial.print(char(item));
  }
}


void IRAM_ATTR addToSendQueue(uint8_t * buff, uint8_t length) {

  for (int i = 0; i < length; i++) {
    UDPoutQue[UDPoutQuePtr] = buff[i];
    UDPoutQuePtr++;

    BluetoothBuffOut[BluetoothBuffOutPointer] = buff[i];
    BluetoothBuffOutPointer++;


    if (MirrorToSerial) {
      Serial.print(char(buff[i]));
    }
  }
}


void IRAM_ATTR HandleServerUDP() {

  SendUDPpacket(); //Send Back any reply or actions that needed to be taken

  int packetSize = UDPserver.parsePacket();
  if (packetSize) {
    int len = UDPserver.read(packetBuffer, 255);
    if (len > 0) packetBuffer[len] = 0;
    Serial.print(packetBuffer);
    memcpy(UDPin, packetBuffer, len);
    uint8_t ControlPacket = UDPin[0];
    uint8_t NodeAddr = UDPin[1];
    handleSerialControlInput(UDPin, ControlPacket, NodeAddr, len);
  }
}

