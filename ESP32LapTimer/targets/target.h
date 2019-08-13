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
#ifndef VRX_SCK
#define VRX_SCK -1
#endif
#ifndef VRX_MOSI
#define VRX_MOSI -1
#endif
#ifndef VRX_MISO
#define VRX_MISO -1
#endif
