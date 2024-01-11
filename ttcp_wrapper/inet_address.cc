#include "inet_address.h"

#include <assert.h>
#include <glog/logging.h>
#include <memory.h>
#include <netdb.h>


namespace socket {

InetAddress::InetAddress() {
  memset(&addr_, 0, sizeof(addr_));
  addr_.sin_family = AF_UNSPEC;
}

// for connecting
InetAddress::InetAddress(std::string ip, uint16_t port) {
  memset(&addr_, 0, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_port = htons(port);
  int result = inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr);
  assert(result == 1);
}

// for listening
InetAddress::InetAddress(uint16_t port) {
  memset(&addr_, 0, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_port = htons(port);
  addr_.sin_addr.s_addr = htonl(INADDR_ANY);
}

// for Sockets API
InetAddress::InetAddress(const struct sockaddr& saddr) {
  memset(&addr_, 0, sizeof(addr_));
  assert(saddr.sa_family == AF_INET);
  memcpy(&addr_, &saddr, sizeof(addr_));
}

bool InetAddress::operator==(const InetAddress& rhs) const {
  return (addr_.sin_family == rhs.addr_.sin_family)
         && (addr_.sin_port == rhs.addr_.sin_port)
         && (addr_.sin_addr.s_addr == rhs.addr_.sin_addr.s_addr);
}

std::string InetAddress::ToIp() const {
  char buf[INET_ADDRSTRLEN];
  const char* result = inet_ntop(AF_INET, &addr_.sin_addr, buf, INET_ADDRSTRLEN);
  assert(result != nullptr);
  return buf;
}

std::string InetAddress::ToIpPort() const {
  return ToIp() + ":" + std::to_string(port());
}

// static function
bool InetAddress::Resolve(std::string hostname, uint16_t port, InetAddress* address) {
  struct hostent* he = gethostbyname(hostname.c_str());
  if (!he) {
    LOG(ERROR) << "Failed to parse hostname: " << hostname;
    perror("gethostbyname: ");
    return false;
  }
  assert(he->h_addrtype == AF_INET && he->h_length == sizeof(uint32_t));
  
  struct sockaddr_in tmp_addr;
  memset(&tmp_addr, 0, sizeof(tmp_addr));
  tmp_addr.sin_family = AF_INET;
  tmp_addr.sin_port = htons(port);
  tmp_addr.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);

  *address = InetAddress(*reinterpret_cast<const struct sockaddr*>(&tmp_addr));
}

} // namespace socket
