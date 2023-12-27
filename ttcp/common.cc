#include "common.h"

#include <assert.h>
#include <netdb.h>
#include <getopt.h>
#include <glog/logging.h>


static void DisplayUsage() {
  LOG(INFO) << "Usage: ttcp \n"
        << "[OPTION]...\n"
        << "Description: \n"
        << "    -h, --help: help information \n"
        << "    -p, --port: TCP port\n"
        << "    -l, --length: Buffer length \n"
        << "    -n, --number: Number of buffers \n"
        << "    -t, --trans: Transmit \n"
        << "    -r, --recv: Receive \n"
        << "    -D, --nodelay: Set TCP_NODELAY \n";
  return;
}

bool ParseCommand(int argc, char * const argv[], Options& opt) {
  opterr = 0;
  const std::string short_opts = "hp:l:n:t:rD";
  static const struct option long_opts[] = {
    {"help", no_argument, nullptr, 'h'},
    {"port", required_argument, nullptr, 'p'},
    {"length", required_argument, nullptr, 'l'},
    {"number", required_argument, nullptr, 'n'},
    {"trans", no_argument, nullptr, 't'},
    {"recv", no_argument, nullptr, 'r'},
    {"nodelay", no_argument, nullptr, 'D'},
    {nullptr, no_argument, nullptr, 0}
  };
  
  int op = 0;
  while ((op = getopt_long(argc, argv, short_opts.c_str(), long_opts, nullptr)) != -1) {
    try {
      switch (op) {
        case 'p':
          opt.port = static_cast<uint16_t>(std::stoul(optarg));
          break;
        case 'l':
          opt.length = std::stoul(optarg);
          break;
        case 'n':
          opt.number = std::stoul(optarg);
          break;
        case 't':
          opt.transmit = true;
          opt.host = std::string(optarg);
          break;
        case 'r':
          opt.receive = true;
          break;
        case 'D':
          opt.nodelay = true;
          break;
        case 'h':
          DisplayUsage();
          exit(0);    
        default:
          LOG(ERROR) << "Invalid argument, please check input.";
          DisplayUsage();
          exit(0);
      }
    } catch (const std::invalid_argument& ex) {
      LOG(ERROR) << "Invalid Argument Error.";
      DisplayUsage();
      exit(0);
    }
  }

  if (opt.transmit && opt.receive) {
    LOG(ERROR) << "-t and -r can not be set at the same time.";
    return false;
  } else if (!opt.transmit && !opt.receive) {
    LOG(ERROR) << "either -t or -r must be specified.";
    return false;
  }

  return true;
}

// parse host and make a sockaddr_in
struct sockaddr_in ResolveOrDie(const char* host, uint16_t port) {
  struct hostent* he = ::gethostbyname(host);
  if (!he) {
    LOG(ERROR) << "Failed to parse host: " << host;
    perror("gethostbyname");
    exit(1);
  }
  assert(he->h_addrtype == AF_INET && he->h_length == sizeof(uint32_t));

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
  return addr;
}

// return TCP connected socket-id 
int AcceptOrDie(uint16_t port) {
  int listen_fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (listen_fd < 0) {
    LOG(ERROR) << "AcceptOrDie: listen_fd is invalid";
    perror("socket");
    exit(1);
  }
  
  int yes = 1;
  if (::setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))) {
    LOG(ERROR) << "Failed to set listen_fd option";
    perror("setsockopt");
    ::close(listen_fd);
    exit(1);
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = ::htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (::bind(listen_fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr))) {
    LOG(ERROR) << "Failed to bind listen_fd";
    perror("bind");
    ::close(listen_fd);
    exit(1);
  }

  struct sockaddr_in peer_addr;
  memset(&peer_addr, 0, sizeof(peer_addr));
  socklen_t addr_len = 0;
  int peer_fd = ::accept(listen_fd, reinterpret_cast<struct sockaddr*>(&peer_addr), &addr_len);
  if (peer_fd < 0) {
    LOG(ERROR) << "Failed to 'accept' peer_fd";
    perror("accept");
    ::close(listen_fd);
    exit(1);
  }

  ::close(listen_fd);
  return peer_fd;
}