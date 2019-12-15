#include "Output.h"

#include "Comms.h"
#include "UDP.h"
#include "Serial.h"
#ifdef USE_BLUETOOTH
#include "Bluetooth.h"
#endif

#define MAX_OUTPUT_BUFFER_SIZE 1500

static uint8_t output_buffer[MAX_OUTPUT_BUFFER_SIZE];
static int output_buffer_pos = 0; //Keep track of where we are in the Que

// TODO: define this somewhere else!
static output_t outputs[] = {
  {NULL, udp_init, udp_send_packet, udp_update, output_input_callback},
#ifdef USE_SERIAL_OUTPUT
  {NULL, serial_init, serial_send_packet, serial_update, output_input_callback}
#endif // USE_SERIAL_OUTPUT
#ifdef USE_BLUETOOTH
  {NULL, bluetooth_init, bluetooth_send_packet, bluetooth_update, output_input_callback},
#endif // USE_BLUETOOTH
};

#define OUTPUT_SIZE (sizeof(outputs)/sizeof(outputs[0]))


bool IRAM_ATTR addToSendQueue(uint8_t item) {
  if(output_buffer_pos >= MAX_OUTPUT_BUFFER_SIZE) {
    Serial.println("Output buffer full!");
    return false;
  }
  output_buffer[output_buffer_pos++] = item;
  return true;
}

uint8_t IRAM_ATTR addToSendQueue(uint8_t * buff, uint32_t length) {
  for (int i = 0; i < length; ++i) {
    if(!addToSendQueue(buff[i])) {
      return i;
    }
  }
  return length;
}

void update_outputs() {
  // First update all inputs before sending new stuff
  for(int i = 0; i < OUTPUT_SIZE; ++i) {
    if(outputs[i].update){
      outputs[i].update(&outputs[i]);
    }
  }
  if(output_buffer_pos >0) {
    // Send current buffer to all configured outputs
    for(int i = 0; i < OUTPUT_SIZE; ++i) {
      if(outputs[i].sendPacket) {
        outputs[i].sendPacket(&outputs[i], output_buffer, output_buffer_pos);
      }
    }
    // TODO: potential race condition? add mutex
    output_buffer_pos = 0;
  }
}

void init_outputs() {
  for(int i = 0; i < OUTPUT_SIZE; ++i) {
    if(outputs[i].init) {
      outputs[i].init(&outputs[i]);
    }
  }
}

void output_input_callback(uint8_t* buf, uint32_t size) {
  uint8_t ControlPacket = buf[0];
  uint8_t NodeAddr = buf[1];
  handleSerialControlInput((char*)buf, ControlPacket, NodeAddr, size);
}
