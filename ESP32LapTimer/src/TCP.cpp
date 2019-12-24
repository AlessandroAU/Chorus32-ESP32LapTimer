#include "TCP.h"

#include "HardwareConfig.h"
#include "Output.h"

#include <lwip/sockets.h>
#include <lwip/netdb.h>

static int tcp_server = -1;

#define TCP_BUF_LEN 1500
static uint8_t packetBuffer[TCP_BUF_LEN];

static int tcpClients[MAX_TCP_CLIENTS];

void tcp_init(void* output) {
  if ((tcp_server=socket(AF_INET, SOCK_STREAM, 0)) == -1){
    return;
  }

  int yes = 1;
  if (setsockopt(tcp_server,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
      close(tcp_server);
      tcp_server = -1;
      return;
  }

  struct sockaddr_in addr;
  memset((char *) &addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(9000);
  addr.sin_addr.s_addr = INADDR_ANY;
  if(bind(tcp_server , (struct sockaddr*)&addr, sizeof(addr)) == -1){
    close(tcp_server);
    tcp_server = -1;
    return;
  }
  fcntl(tcp_server, F_SETFL, O_NONBLOCK);
  listen(tcp_server, MAX_TCP_CLIENTS);
  for(int i = 0; i < MAX_TCP_CLIENTS; ++i) {
    tcpClients[i] = -1;
  }
  log_i("Created tcp socket");
}

void IRAM_ATTR tcp_send_packet(void* output, uint8_t* buf, uint32_t size) {
  if(tcp_server < 0) return;
  if (buf != NULL && size != 0) {
    for(int i = 0; i < MAX_TCP_CLIENTS; ++i) {
      if(tcpClients[i] >= 0) {
        ::send(tcpClients[i], buf, size, 0);
      }
    }
  }
}

void IRAM_ATTR tcp_update(void* output) {
  if(tcp_server < 0) return;
  // check for new connections
  struct sockaddr_storage clientAddress;
  socklen_t size = sizeof(clientAddress);
  int new_socket = accept(tcp_server, (struct sockaddr*)&clientAddress, &size);
  if(new_socket > 0) { // new client
    int i = 0;
    for(i = 0; i < MAX_TCP_CLIENTS; ++i) {
      if(tcpClients[i] < 0) { // free spot
        tcpClients[i] = new_socket;
        fcntl(new_socket, F_SETFL, O_NONBLOCK);
        log_d("Got new tcp client!");
        break;
      }
    }
    // No space for the new client found
    if(i == MAX_TCP_CLIENTS) {
      log_w("Got new tcp client, but no free space is available!");
      ::close(new_socket);
    }
  }
  // Receive the data and remove broken connections
  for(int i = 0; i < MAX_TCP_CLIENTS; ++i) {
    if(tcpClients[i] >= 0) {
      int len = recv(tcpClients[i], packetBuffer, TCP_BUF_LEN - 1, 0);
      if (len == 0){ // client shut down
        log_d("Removed tcp client due to len == 0");
        tcpClients[i] = -1;
      } else if (len < 0) { // other error
        if(errno != EWOULDBLOCK && errno != EAGAIN) {
          log_d("tcp error: %d", errno);
          // Remove client on all other errors just in case
          close(tcpClients[i]);
          tcpClients[i] = -1;
        }
      } else { // got new data
        output_t* out = (output_t*)output;
        packetBuffer[len] = 0;
        out->handle_input_callback(packetBuffer, len);
      }
    }
  }
}
