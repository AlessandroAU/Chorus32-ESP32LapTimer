#include "TCP.h"

void IRAM_ATTR SendUDPpacket() {

  if (UDPoutQuePtr > 0) {

    IPAddress remoteIp = UDPserver.remoteIP();
    uint16_t port = UDPserver.remotePort();
    UDPserver.beginPacket(remoteIp, port);

    UDPserver.write((const uint8_t *)UDPoutQue, UDPoutQuePtr);

    //    for (int i = 0; i < UDPoutQuePtr; i++) {
    //      if (MirrorToSerial) {
    //        Serial.print((char)UDPoutQue[i]);
    //      }
    //    }



    UDPserver.endPacket();
    //Serial.println("");
    UDPoutQuePtr = 0;


    //delay(1);
  }
}


char SerialBuffIn[100];

byte ndx = 0;
char rc;
byte StartOfLastCMD = 0;

void IRAM_ATTR ProcessSerialCommand(char * BuffIn, byte StartIndex, byte Length) {
  //
  char DatatoProcess[20];

  memcpy(DatatoProcess, &BuffIn[+StartIndex], Length);
  //
  uint8_t ControlPacket = DatatoProcess[0];   ///fix this when you have the chance and seperate the serial and UDP functions
  uint8_t NodeAddr = DatatoProcess[1];

  handleSerialControlInput(DatatoProcess, ControlPacket, NodeAddr, Length);

  //  for (int i = 0; i < Length; i++) {
  //    Serial.print(DatatoProcess[i]);
  //  }

}

void IRAM_ATTR HandleSerialRead() {




  //  if (!Serial.available()) {
  //    ndx = 0;
  //  }

  while (Serial.available() > 0 ) {
    rc = Serial.read();
    SerialBuffIn[ndx] = rc;
    ndx++;
  }

  for (int i = 0; i < ndx; i++) {
    if (SerialBuffIn[i] == endMarker) {
      for (int j = 0; j <= i; j++) {
        //Serial.print(SerialBuffIn[j]);
        ProcessSerialCommand(SerialBuffIn, 0, i + 1);
        break;
      }
      memcpy(SerialBuffIn, &SerialBuffIn[i + 1], 50 - i);
      for (int i = 0; i < 20; i++) {
        if (SerialBuffIn[i] == 0)
          ndx = i;
        break;
      }
      return;
    }
  }
}



//for (int i = 0; i < ndx; i++) {
//
//  char inChar = SerialBuffIn[ndx];
//  Serial.println(inChar);
//
//  if (inChar == endMarker) {
//
//    //do stuff
//    uint8_t ControlPacket = SerialBuffIn[StartOfLastCMD];   ///fix this when you have the chance and seperate the serial and UDP functions
//    uint8_t NodeAddr = SerialBuffIn[StartOfLastCMD + 1];
//
//    memcpy(inData, SerialBuffIn + StartOfLastCMD, i - StartOfLastCMD);
//
//    handleSerialControlInput(inData, ControlPacket, NodeAddr, i - StartOfLastCMD);
//
//    Serial.println(ControlPacket);
//    Serial.println(NodeAddr);
//    Serial.println(inData);
//
//    StartOfLastCMD = i;
//  }
//
//}
////ndx = 0;
//}

//if (rc != endMarker) {
//  SerialBuffIn[ndx] = rc;
//  ndx++;
//  //      if (ndx >= numChars) {
//  //        ndx = numChars - 1;
//  //      }
//}
//else {
//  //memcpy(SerialBuffIn, packetBuffer, len);
//  uint8_t ControlPacket = SerialBuffIn[0];   ///fix this when you have the chance and seperate the serial and UDP functions
//  uint8_t NodeAddr = SerialBuffIn[1];
//  handleSerialControlInput(SerialBuffIn, ControlPacket, NodeAddr, ndx);
//
//  //receivedChars[ndx] = '\0'; // terminate the string
//
//  //newData = true;
//}
//}
//}




void IRAM_ATTR HandleServerUDP() {

  if (RemoteClient.connected()) {  // if we have a TCP connecting going don't do any UDP stuff
    return;
  }

  SendUDPpacket(); //Send Back any reply or actions that needed to be taken

  int packetSize = UDPserver.parsePacket();
  if (packetSize > 0) {
    int len = UDPserver.read(packetBuffer, 255);
    if (len > 0) packetBuffer[len] = 0;
    Serial.print(packetBuffer);
    memcpy(UDPin, packetBuffer, len);
    uint8_t ControlPacket = UDPin[0];
    uint8_t NodeAddr = UDPin[1];
    handleSerialControlInput(UDPin, ControlPacket, NodeAddr, len);
  }
}
