#include "Queue.h"

#include <esp_attr.h>
#include <string.h>

void queue_init(queue_t* queue, void** data, uint32_t max_size) {
  queue->curr_size = 0;
  queue->data = data;
  queue->max_size = max_size;
}

int queue_empty(queue_t* queue){
  return queue->curr_size != 0;
}

void* queue_get(queue_t* queue, uint32_t index) {
  if(index < queue->curr_size) {
    return queue->data[index];
  }
  return NULL;
}

void* IRAM_ATTR queue_peek(queue_t* queue) {
  if(queue == NULL){
    return NULL;
  }
  if(queue->curr_size == 0) {
    return NULL;
  }
  void* data = queue->data[0];
  return data;
}

void* IRAM_ATTR queue_dequeue(queue_t* queue){
  void* data = queue_peek(queue);
  if(data) {
    --queue->curr_size;
    memmove(queue->data, queue->data + 1, sizeof(void*) * queue->curr_size);
  }
  return data;
}


int IRAM_ATTR queue_enqueue(queue_t* queue, void* data){
  if(queue == NULL){
    return -1;
  }
  if(queue->curr_size + 1 > queue->max_size) {
    return QUEUE_ERROR_FULL;
  }
  queue->data[queue->curr_size] = data;
  queue->curr_size += 1;
  return 0;
}
