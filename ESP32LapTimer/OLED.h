void oledSetup();
void oledUpdate();
void OLED_CheckIfUpdateReq();
uint16_t displayScreenNumber = 0;
uint8_t  numberOfBaseScreens = 4; // Increment if a new screen is added to cycle through.
// The actual number of screens will be calculated on Button 1 press in buttons.ino.
