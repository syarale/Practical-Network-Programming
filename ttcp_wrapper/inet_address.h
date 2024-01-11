#ifndef TTCP_WRAPPER_H_
#define TTCP_WRAPPER_H_

#include <arpa/inet.h>
#include <string>

namespace socket {

class InetAddress {
 public:
  InetAddress();
  explicit InetAddress(std::string ip, uint16_t port);   // for connecting
  explicit InetAddress(uint16_t port);                   // for listening
  explicit InetAddress(const struct sockaddr& saddr);    // for sockets API

  InetAddress(const InetAddress&) = default;
  InetAddress& operator= (const InetAddress&) = default;
  ~InetAddress() = default;

  bool operator==(const InetAddress& rhs) const;
  
  uint16_t port() const { return ntohs(addr_.sin_port);}
  void set_port(uint16_t port) {addr_.sin_port = htons(port);}

  std::string ToIp() const;
  std::string ToIpPort() const;
  sa_family_t sin_family() const { return addr_.sin_family; }

  socklen_t length() const { return sizeof(addr_); }

  const struct sockaddr* GetSockAddr() const {
    return reinterpret_cast<const struct sockaddr*>(&addr_);
  }

  static bool Resolve(std::string hostname, uint16_t port, InetAddress*);

 private:
  struct sockaddr_in addr_;  // only supports ipv4
};

} // namespace socket
#endif // TTCP_WRAPPER_H_ 
