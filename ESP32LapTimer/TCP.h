#ifndef __TCP_H__
#define __TCP_H__

#include "Output.h"

#include <Arduino.h>
#include <stdint.h>

void IRAM_ATTR tcp_send_packet(void* output, uint8_t* buf, uint32_t size);
void tcp_init(void* output);
void IRAM_ATTR tcp_update(void* output);

#endif // __TCP_H__
