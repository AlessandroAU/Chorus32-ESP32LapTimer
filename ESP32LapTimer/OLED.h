void oledSetup();
void oledUpdate();
void OLED_CheckIfUpdateReq();
uint16_t displayScreenNumber = 0;
uint8_t  numberOfOledScreens = 3; // Increment if a new screen is added to cycle through.
