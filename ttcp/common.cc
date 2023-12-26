#include "common.h"

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