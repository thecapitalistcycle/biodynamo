#ifndef __WORKER_COMM__
#define __WORKER_COMM__

#include <string>
#include <vector>

#include <zmqpp/zmqpp.hpp>

#include "common.h"
#include "communicator.h"

namespace bdm {

class WorkerCommunicator : public Communicator {
 public:
  WorkerCommunicator(DistSharedInfo* info, const std::string& endpoint,
                     CommunicatorId comm_id);
  ~WorkerCommunicator();

  void Connect();
  void HandleOutgoingMessage(zmqpp::message& msg);
  void HandleIncomingMessage();

 private:
  void SendToCoWorker(const std::string& command,
                      zmqpp::message* message = nullptr,
                      const std::string& option = "");

  bool client_;  // Act as client? (aka initiate communication)
  std::string coworker_identity_;

  std::string coworker_str_;
  std::string worker_str_;
};
}

#endif  //__WORKER_COMM__