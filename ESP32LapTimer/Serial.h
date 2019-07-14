#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <stdint.h>

void serial_init(void* output);
void serial_update(void* output);
void serial_send_packet(void* output, uint8_t* buf, uint32_t size);

#endif // __SERIAL_H__
