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
      LOG(INFO) << "Start ttcp-client ...";
      Transmit(opt);
    } else if (opt.receive) {
      LOG(INFO) << "Start ttcp-server ...";
      Receive(opt);
    } else {
      assert(0);
    }
  } else {
    LOG(ERROR) << "Failed to Parse parameter, exit later.";
    return 0;
  }

  google::ShutdownGoogleLogging();
  return 0;
}