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

bool ParseCommand(int argc , char * const argv[], Options& opt);



#endif  // TTCP_COMMON_H_