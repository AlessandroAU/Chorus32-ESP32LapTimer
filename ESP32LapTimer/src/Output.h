#ifndef __OUTPUT_H__
#define __OUTPUT_H__

#include <stdint.h>
#include <Arduino.h>


typedef struct output_s {
  void* data;
  void (*init)(void* output);
  void (*sendPacket)(void* output, uint8_t* buf, uint32_t size);
  void (*update)(void* output);
  void (*handle_input_callback)(uint8_t* buf, uint32_t size);
} output_t;

/**
 * \return true if the item has been added to the queue
 */
bool IRAM_ATTR addToSendQueue(uint8_t item);

/**
 * \return the number of items added to the queue
 */
uint8_t IRAM_ATTR addToSendQueue(uint8_t * buff, uint32_t length);

void update_outputs();
void init_outputs();


void output_input_callback(uint8_t* buf, uint32_t size);


#endif // __OUTPUT_H__
