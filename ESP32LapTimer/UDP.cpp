#include "UDP.h"

#include "HardwareConfig.h"
#include "Output.h"

#include <WiFi.h>
#include <WiFiUdp.h>

static WiFiUDP UDPserver;

static uint8_t packetBuffer[1500];

static struct udp_source_s {
  IPAddress addr;
  uint16_t port;
} udpClients[MAX_UDP_CLIENTS];

void add_ip_port() {
  IPAddress remoteIp = UDPserver.remoteIP();
  uint16_t port = UDPserver.remotePort();
  // if current ip is already known move it to the front. or if on the last entry delete it to make room
  for(int i = 0; i < MAX_UDP_CLIENTS; ++i) {
    if((udpClients[i].addr == remoteIp && udpClients[i].port == port) || (i+1 == MAX_UDP_CLIENTS)) {
      memmove(udpClients + 1, udpClients, i * sizeof(udpClients[0]));
      break;
    }
  }
  udpClients[0].addr = remoteIp;
  udpClients[0].port = port;
}

void udp_init(void* output) {
  UDPserver.begin(9000);
}

void IRAM_ATTR udp_send_packet(void* output, uint8_t* buf, uint32_t size) {
  if (buf != NULL && size != 0) {
    for(int i = 0; i < MAX_UDP_CLIENTS; ++i) {
      if(udpClients[i].addr != 0) {
        UDPserver.beginPacket(udpClients[i].addr, udpClients[i].port);
        UDPserver.write(buf, size);
        UDPserver.endPacket();
      }
    }
  }
}

void IRAM_ATTR udp_update(void* output) {
  int packetSize = UDPserver.parsePacket();
  if (packetSize > 0) {
    add_ip_port();
    int len = UDPserver.read(packetBuffer, 255);
    if (len > 0) packetBuffer[len] = 0;
    output_t* out = (output_t*)output;
    out->handle_input_callback(packetBuffer, len);
  }
}
