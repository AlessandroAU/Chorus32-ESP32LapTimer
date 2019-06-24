#include "Serial.h"

#include "Output.h"

#include <Arduino.h>
#include <stdint.h>

static char rc;
static char endMarker = '\n';
static char SerialBuffIn[50];
static uint8_t ndx = 0;

void IRAM_ATTR ProcessSerialCommand(void* output, char * BuffIn, uint8_t StartIndex, uint8_t Length) {
  uint8_t DatatoProcess[20];

  memcpy(DatatoProcess, &BuffIn[+StartIndex], Length);
  output_t* out = (output_t*)output;
  out->handle_input_callback(DatatoProcess, Length);
}

void serial_update(void* output) {
  while (Serial.available() > 0 ) {
    rc = Serial.read();
    SerialBuffIn[ndx] = rc;
    ndx++;
  }

  for (int i = 0; i < ndx; i++) {
    if (SerialBuffIn[i] == endMarker) {
      for (int j = 0; j <= i; j++) {
        ProcessSerialCommand(output, SerialBuffIn, 0, i + 1);
        break;
      }
      memcpy(SerialBuffIn, &SerialBuffIn[i + 1], 50 - i);
      for (int i = 0; i < 20; i++) {
      if (SerialBuffIn[i] == 0)
        ndx = i;
        break;
      }
      return;
    }
  }
}

void serial_init(void* output) {
  (void)output;
}

void serial_send_packet(void* output, uint8_t* buf, uint32_t size) {
  Serial.write(buf, size);
}
