void IRAM_ATTR addToSendQueue(uint8_t item);
void IRAM_ATTR addToSendQueue(uint8_t * buff, uint8_t length);

char packetBuffer[255];
char UDPin[255];

uint8_t UDPoutQue[255];
int UDPoutQuePtr = 0; //Keep track of where we are in the Que

bool MirrorToSerial = true;
void IRAM_ATTR SendUDPpacket();
