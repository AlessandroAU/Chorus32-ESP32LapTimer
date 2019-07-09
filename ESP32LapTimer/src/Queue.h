#ifndef __QUEUE_H__
#define __QUEUE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define QUEUE_ERROR_FULL -2
#define QUEUE_ERROR_EMPTY -3

typedef struct queue_s{
  void** data;
  uint8_t curr_size;
  uint32_t max_size;
} queue_t;

void queue_init(queue_t* queue, void** data, uint32_t max_size);
int queue_empty(queue_t* queue);
void* queue_get(queue_t* queue, uint32_t index);
void* queue_dequeue(queue_t* queue);
int queue_enqueue(queue_t* queue, void* data);
void* queue_peek(queue_t* queue);


#ifdef __cplusplus
}
#endif

#endif // __QUEUE_H__
