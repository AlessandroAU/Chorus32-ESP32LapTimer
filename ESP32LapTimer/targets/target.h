#if BOARD==BOARD_DEFAULT
  #include "config_default.h"
#elif BOARD==BOARD_OLD
  #include "config_old.h"
#elif BOARD==BOARD_TTGO_LORA
  #include "config_ttgo_lora_v1.h"
#else 
  #error "Unknown board!"
#endif

#if defined(BUTTON1) && defined(BUTTON2)
#define USE_BUTTONS
#endif

// Define unconfigured pins
#ifndef SCK
#define SCK -1
#endif
#ifndef MOSI
#define MOSI -1
#endif
#ifndef MISO
#define MISO -1
#endif
