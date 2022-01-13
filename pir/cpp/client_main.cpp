#include "pir/cpp/client.h"

// add by byte
#include "pir/proto/payload.grpc.pb.h"
#include <grpcpp/grpcpp.h>


// add by byte
using grpc::Channel;
// using grpc::Status;
using pir::Request;
using pir::Response;
using pir::Query;
using grpc::ClientContext;
using pir::Ciphertexts;



class PirClient {
 public:
  PirClient(std::shared_ptr<Channel> channel)
      : stub_(Query::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  std::string SendPubkeyA(const std::string& query) {
    // Data we are sending to the server.
    Request request;
    Ciphertexts* ct;
    ct = request.add_query();
    std::cout << "test1" << std::endl;
    ct->set_ct(1, query.data());
    std::cout << "test2" << std::endl;

    // Container for the data we expect from the server.
    Response reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    stub_->sendQuery(&context, request, &reply);
  }

 private:
  std::unique_ptr<Query::Stub> stub_;
};

int main() {
  printf("sender starting..");
  int ret = 0;

  std::string target_str = "localhost:50051";
  PirClient pirclient(grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
  std::string pubkeyA("123456");
  std::string reply = pirclient.SendPubkeyA(pubkeyA);
  std::cout << "ot12 pubA reply = " <<  reply  << std::endl;

  printf("have sendt pubkey to client.\n");

  return 0;
}