#include <assert.h>
#include <memory>

#include <glog/logging.h>
#include "common.h"

int main(int argc, char* argv[]) {
  Options opt;

  google::InitGoogleLogging(argv[0]);
  google::SetStderrLogging(google::INFO);
  FLAGS_colorlogtostderr = true;
  
  if (ParseCommand(argc, argv, opt)) {
    if (opt.transmit) {
      LOG(INFO) << "Transmit is true, " << opt.host;
    } else if (opt.receive) {
      LOG(INFO) << "Receive is true";
    } else {
      assert(0);
    }
  } else {
    LOG(ERROR) << "Failed to Parse";
    return 0;
  }

  LOG(WARNING) << "port: " << opt.port << "\n"
            << "length: " << opt.length << "\n"
            << "number: " << opt.number << "\n";

  google::ShutdownGoogleLogging();
  return 0;
}