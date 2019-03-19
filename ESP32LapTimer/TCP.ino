

//void HandleServerTCP() {
//  WiFiClient client = wifiServer.available();
//
//  if (client) {
//
//    while (client.connected()) {
//
//      while (client.available() > 0) {
//        char c = client.read();
//        client.write(c);
//        Serial.print(c);
//      }
//    }
//
//    client.stop();
//    Serial.println("Client disconnected");
//
//  }
//
//}

uint32_t checkClientsInterval = 10;
uint32_t LastClientCheck;


void TCPinit() {
  wifiServer.begin();
}

WiFiClient RemoteClient;

void CheckForConnections() {

  if (millis() > LastClientCheck + checkClientsInterval) {
    //Serial.println("check clinents");


    if (wifiServer.hasClient())
    {
      webServer.stop();
      RemoteClient = wifiServer.available();
      

      //      // If we are already connected to another computer,
      //      // then reject the new connection. Otherwise accept
      //      // the connection.
      //      if (RemoteClient.connected())
      //      {
      //        Serial.println("Connection rejected");
      //        wifiServer.available().stop();
      //      }
      //      else
      //      {
      //        Serial.println("Connection accepted");
      //        RemoteClient = wifiServer.available();
      //        RemoteClient.setNoDelay(1);
      //        //RemoteClient.setTimeout(1);
      //        //delay(1000);
      //      }
      //    }
      LastClientCheck = millis();
    }
  }
}


//
byte TCP_ndx = 0;
char TCP_rc;
byte TCP_StartOfLastCMD = 0;
char endMarker = '\n';

void ProcessTCPCommand(char * BuffIn, byte StartIndex, byte Length) {
  //
  char DatatoProcess[20];

  memcpy(DatatoProcess, &BuffIn[+StartIndex], Length);
  //
  uint8_t ControlPacket = DatatoProcess[0];   ///fix this when you have the chance and seperate the serial and UDP functions
  uint8_t NodeAddr = DatatoProcess[1];

  Serial.print("TCP_in: " );
  for (int i = 0; i < Length; i++) {

    Serial.print(DatatoProcess[i]);
  }
  //Serial.print('\n');

  handleSerialControlInput(DatatoProcess, ControlPacket, NodeAddr, Length);

  //  for (int i = 0; i < Length; i++) {
  //    Serial.print(DatatoProcess[i]);
  //  }

}

void HandleServerTCP() {

  //delay(20);

  CheckForConnections();

  SendTCPpacket();

  //  if (!Serial.available()) {
  //    ndx = 0;
  //  }

  //WiFiClient TCPclient = wifiServer.available();

  if (RemoteClient) {

    if (RemoteClient.connected()) {

      while (RemoteClient.available()) {
        char c = RemoteClient.read();
        //Serial.print(c);
        TCP_rc = c;
        TCPBuffIn[TCP_ndx] = TCP_rc;

        if (TCPBuffIn[TCP_ndx] == endMarker) {
          ProcessTCPCommand(TCPBuffIn, 0, TCP_ndx);
          TCP_ndx = 0;
          break;
        } else {
          TCP_ndx++;
        }

        //client.stop();
        //Serial.println("TCP Client disconnected");
      }
    }
  }
}
//    while (Serial.available() > 0 ) {
//      TCP_rc = Serial.read();
//      TCPBuffIn[TCP_ndx] = TCP_rc;
//      TCP_ndx++;
//    }
//
//  for (int i = 0; i < TCP_ndx; i++) {
//    if ((TCPBuffIn[i] == endMarker) and (i > 2)) {
//      for (int j = 0; j <= i; j++) {
//        //Serial.print(SerialBuffIn[j]);
//        ProcessTCPCommand(TCPBuffIn, 0, i + 1);
//        break;
//      }
//      memcpy(TCPBuffIn, &TCPBuffIn[i + 1], 50 - i);
//      for (int i = 0; i < 20; i++) {
//        if (TCPBuffIn[i] == 0)
//          TCP_ndx = i;
//        break;
//      }
//      return;
//    }
//  }
//}

void SendTCPpacket() {


  //WiFiClient TCPclient = wifiServer.available();

  if (RemoteClient) {

    if (RemoteClient.connected()) {

      if (TCPoutQuePtr > 0) {
        Serial.print(" Send Bytes: ");
        Serial.println(RemoteClient.write((const uint8_t *)TCPoutQue, TCPoutQuePtr));
        Serial.println("TCP_resp: ");

        for (int i = 0; i < TCPoutQuePtr; i++) {
          //if (MirrorToSerial) {
          Serial.print((char)TCPoutQue[i]);
          //}
        }

        //RemoteClient.stop();
      }
      TCPoutQuePtr = 0;
    }
    //client.stop();
    //Serial.println("Client disconnected (no client?)");
  }


  //  if ((UDPoutQuePtr > 0) and wifiServer.connected()) {
  //
  //    IPAddress remoteIp = UDPserver.remoteIP();
  //    uint16_t port = UDPserver.remotePort();
  //    UDPserver.beginPacket(remoteIp, port);
  //
  //    for (int i = 0; i < UDPoutQuePtr; i++) {
  //      if (MirrorToSerial) {
  //        Serial.print((char)UDPoutQue[i]);
  //      }
  //    }
  //
  //    UDPserver.write((const uint8_t *)UDPoutQue, UDPoutQuePtr);
  //
  //    UDPserver.endPacket();
  //    //Serial.println("");
  //    UDPoutQuePtr = 0;


  //delay(1);
  //}
}
