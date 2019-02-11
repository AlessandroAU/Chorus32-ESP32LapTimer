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

#ifdef BluetoothEnabled
  BluetoothBuffOut[BluetoothBuffOutPointer] = item;
  BluetoothBuffOutPointer++;
#endif

  if (MirrorToSerial) {
    Serial.print(char(item));
  }
}


void IRAM_ATTR addToSendQueue(uint8_t * buff, uint8_t length) {

  for (int i = 0; i < length; i++) {
    UDPoutQue[UDPoutQuePtr] = buff[i];
    UDPoutQuePtr++;

#ifdef BluetoothEnabled
    BluetoothBuffOut[BluetoothBuffOutPointer] = buff[i];
    BluetoothBuffOutPointer++;
#endif

    if (MirrorToSerial) {
      Serial.print(char(buff[i]));
    }
  }
}

char SerialBuffIn[20];

void IRAM_ATTR HandleSerialRead() {
  byte ndx = 0;
  char endMarker = '\n';
  char rc;

  while (Serial.available() > 0) {
    rc = Serial.read();

    if (rc != endMarker) {
      SerialBuffIn[ndx] = rc;
      ndx++;
//      if (ndx >= numChars) {
//        ndx = numChars - 1;
//      }
    }
    else {
      //memcpy(SerialBuffIn, packetBuffer, len);
      uint8_t ControlPacket = SerialBuffIn[0];   ///fix this when you have the chance and seperate the serial and UDP functions
      uint8_t NodeAddr = SerialBuffIn[1];
      handleSerialControlInput(SerialBuffIn, ControlPacket, NodeAddr, ndx);
      //receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      //newData = true;
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

