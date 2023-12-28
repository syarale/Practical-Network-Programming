#ifndef TTCP_COMMON_H_
#define TTCP_COMMON_H_


#include <stdint.h>
#include <string>

constexpr uint16_t DEFAULT_PORT = 5001;
constexpr uint32_t DEFAULT_NUMBER = 8192;
constexpr uint32_t DEFAULT_LENGTH = 65536;

struct Options {
  uint16_t port;
  uint32_t length;
  uint32_t number;
  bool transmit, receive, nodelay;
  std::string host;
  Options() : port(DEFAULT_PORT), length(DEFAULT_LENGTH), number(DEFAULT_NUMBER),
              transmit(false), receive(false), nodelay(false) {
  }
};

struct SessionMessage {
  uint32_t number;
  uint32_t length;
} __attribute__((__packed__));

struct PayloadMessage {
  uint32_t length;
  char data[0];  // Flexible Array Member，FAM
};

 
bool ParseCommand(int argc , char * const argv[], Options& opt);
struct sockaddr_in ResolveOrDie(const char* host, uint16_t port);
int AcceptOrDie(uint16_t port);

// implemented in ttcp_blocking.cc 
void Transmit(const Options& opt);
void Receive(const Options& opt);


#endif  // TTCP_COMMON_H_