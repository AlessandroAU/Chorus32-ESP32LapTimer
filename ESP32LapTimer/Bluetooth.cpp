#include "Bluetooth.h"

#include "Output.h"
#include "HardwareConfig.h"

#include <stdint.h>

#define BLUETOOTH_BUFFER_SIZE 255

static uint8_t BluetootBuffIn[BLUETOOTH_BUFFER_SIZE];
static int BluetootBuffInPointer = 0;

static BluetoothSerial SerialBT;

void bluetooth_init(void* output) {
  SerialBT.begin(BLUETOOTH_NAME);
}

void bluetooth_update(void* output) {
  if (SerialBT.available()) {
    if(BluetootBuffInPointer >= BLUETOOTH_BUFFER_SIZE - 1) {
      Serial.println("Bluetooth input buffer full! Reseting...");
      BluetootBuffInPointer = 0;
    }
    BluetootBuffIn[BluetootBuffInPointer] = SerialBT.read();
    if (BluetootBuffIn[BluetootBuffInPointer] == '\n') {
      output_t* out = (output_t*)output;
      out->handle_input_callback(BluetootBuffIn, BluetootBuffInPointer + 1);
      BluetootBuffInPointer = 0;
    } else {
      BluetootBuffInPointer++;
    }
  }
}

void bluetooth_send_packet(void* output, uint8_t* buf, uint32_t size) {
  if (SerialBT.hasClient()) {
    SerialBT.write(buf, size);
  }
}
