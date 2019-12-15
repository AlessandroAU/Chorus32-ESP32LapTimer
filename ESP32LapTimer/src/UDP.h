#ifndef __UDP_H__
#define __UDP_H__

#include "Output.h"

#include <Arduino.h>
#include <stdint.h>

void IRAM_ATTR udp_send_packet(void* output, uint8_t* buf, uint32_t size);
void udp_init(void* output);
void IRAM_ATTR udp_update(void* output);

#endif // __UDP_H__
