#include "common.h"
#include <assert.h>
#include <chrono>
#include <glog/logging.h>

#include <arpa/inet.h>
#include <netinet/in.h>

static uint32_t WriteN(int sockfd, void* buf, uint32_t length) {
  // TODO(leisy)
  return 0;
}

static uint32_t ReadN(int sockfd, void* buf, uint32_t length) {
  // TODO(leisy)
  return 0;
}

void Transmit(const Options& opt) {
  struct sockaddr_in addr = ResolveOrDie(opt.host.c_str(), opt.port);
  LOG(INFO) << "Connecting to " << inet_ntoa(addr.sin_addr) << " : " << opt.port;
  
  int sockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd < 0) {
    LOG(ERROR) << "Transmit: sockfd is invalid";
    perror("socket");
    exit(1);
  }

  int ret = ::connect(sockfd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
  if (ret) {
    LOG(ERROR) << "Failed to connect " << opt.host 
               << " (" << inet_ntoa(addr.sin_addr) << ")";
    perror("connect");
    ::close(sockfd);
    exit(1);
  }
  LOG(INFO) << "connected";
  
  struct SessionMessage session_msg = {0, 0};
  session_msg.length = htonl(opt.length);
  session_msg.number = htonl(opt.number);
  if (WriteN(sockfd, &session_msg, sizeof(SessionMessage)) != sizeof(SessionMessage)) {
    LOG(ERROR) << "Failed to send SessionMessage";
    perror("write SessionMessage");
    exit(1);
  }

  const uint32_t total_len = static_cast<uint32_t>(sizeof(uint32_t) + opt.length);
  struct PayloadMessage* payload_msg_ptr = static_cast<PayloadMessage*>(malloc(total_len));
  assert(payload_msg_ptr);
  
  std::string label = "0123456789ABCDEF";
  for (uint32_t i = 0; i < opt.length; i ++) {
    payload_msg_ptr->data[i] = label[i % 16];
  }

  double total_mb = 1.0 * opt.length * opt.number / (1024 * 1024);  // MB
  LOG(INFO) << total_mb << " MiB in total";
  
  auto start_time = std::chrono::high_resolution_clock::now();
  for (uint32_t i = 0; i < opt.number; i ++) {
    uint32_t nw_payload = WriteN(sockfd, payload_msg_ptr, total_len);
    assert(nw_payload == total_len);

    uint32_t ack = 0;
    uint32_t nr_ack = ReadN(sockfd, &ack, sizeof(ack));
    assert(nr_ack == sizeof(ack));
    ack = ntohl(ack);
    assert(ack == opt.length);
  }
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
  LOG(INFO) << "Cost time: " << duration.count() << ", " 
            << total_mb / duration.count() << " MiB/s"; 

  free(payload_msg_ptr);
  ::close(sockfd);
  return;
}

void Receive(const Options& opt) {
  int sockfd = AcceptOrDie(opt.port);
  
  LOG(INFO) << "Waiting SessionMessage...";
  SessionMessage session_msg = {0, 0};
  uint32_t nr = ReadN(sockfd, &session_msg, sizeof(SessionMessage));
  assert(nr == sizeof(SessionMessage));
  uint32_t length = ntohl(session_msg.length);
  uint32_t number = ntohl(session_msg.number);
  LOG(INFO) << "SessionMessage received, number: " << number << "length: " << length; 
  
  uint32_t total_len = sizeof(uint32_t) + length;
  PayloadMessage* payload_msg_ptr = static_cast<PayloadMessage*>(malloc(total_len));
  assert(payload_msg_ptr);
  
  LOG(INFO) << "Receiving PayloadMessage...";
  for (uint32_t i = 0; i < number; i ++) {
    payload_msg_ptr->length = 0;
    uint32_t nr_len = ReadN(sockfd, &(payload_msg_ptr->length), sizeof(payload_msg_ptr->length));
    assert(nr_len == sizeof(payload_msg_ptr->length));

    payload_msg_ptr->length = ntohl(payload_msg_ptr->length);
    assert(payload_msg_ptr->length == length);

    uint32_t nr_data = ReadN(sockfd, payload_msg_ptr->data, payload_msg_ptr->length);
    assert(nr_data == payload_msg_ptr->length);

    uint32_t ack = htonl(payload_msg_ptr->length);
    uint32_t nw_ack = WriteN(sockfd, &ack, sizeof(ack)); 
    assert(nw_ack == sizeof(ack));
  }
  LOG(INFO) << "Finished";

  ::close(sockfd);
  free(payload_msg_ptr);
  return;
}