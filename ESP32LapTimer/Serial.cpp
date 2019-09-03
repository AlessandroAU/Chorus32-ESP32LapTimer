#include "Serial.h"

#include "Output.h"

#include <Arduino.h>
#include <stdint.h>

#define SERIAL_INPUT_BUF 50

static uint8_t SerialBuffIn[SERIAL_INPUT_BUF];
static uint8_t current_pos = 0;

void serial_update(void* output) {
  while (Serial.available() > 0) {
    char rc = Serial.read();
    SerialBuffIn[current_pos] = rc;
    ++current_pos;
    // Found packet, so handle it
    if(rc == '\n') {
      output_t* out = (output_t*)output;
      out->handle_input_callback(SerialBuffIn, current_pos);
      current_pos = 0;
    } else if(current_pos >= SERIAL_INPUT_BUF) {
      // we are overflowing, so we just discard the current buffer which should just be garbage
      current_pos = 0;
    }
  }
}

void serial_init(void* output) {
  (void)output;
}

void serial_send_packet(void* output, uint8_t* buf, uint32_t size) {
  (void) output;
  Serial.write(buf, size);
}
