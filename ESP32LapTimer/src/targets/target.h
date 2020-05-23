/*
 * This file is part of Chorus32-ESP32LapTimer 
 * (see https://github.com/AlessandroAU/Chorus32-ESP32LapTimer).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#if BOARD==BOARD_DEFAULT
  #include "config_default.h"
#elif BOARD==BOARD_OLD
  #include "config_old.h"
#elif BOARD==BOARD_TTGO_LORA
  #include "config_ttgo_lora_v1.h"
#elif BOARD==BOARD_WROOM
  #include "config_wroom.h"
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

#if !defined(WIFI_MODE_CLIENT) && !defined(WIFI_MODE_ACCESSPOINT)
  #error "No WIFI_MODE selected"
#endif
