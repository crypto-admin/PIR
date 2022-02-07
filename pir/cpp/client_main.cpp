#include "pir/cpp/client.h"
#include "seal/seal.h"
#include "pir/cpp/server_impl.h"

// add by byte
#include "pir/proto/payload.grpc.pb.h"
#include <grpcpp/grpcpp.h>


//
#include "parameters.h"
///////////////

// add by byte
using grpc::Channel;
// using grpc::Status;
using pir::Request;
using pir::Response;
using pir::Query;
using grpc::ClientContext;
using pir::Ciphertexts;
using pir::HelloRequest;
using pir::HelloReply;
using namespace pir; 



class PirQuery {
 public:
  PirQuery(std::shared_ptr<Channel> channel)
      : stub_(Query::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  pir::Response SendQuery(const pir::Request& query) {
 
    // Container for the data we expect from the server.
    Response reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    stub_->sendQuery(&context, query, &reply);

    return reply;
  }

  std::string SayHello(const std::string& user) {
    // Data we are sending to the server.
    HelloRequest request;
    request.set_name(user);

    // Container for the data we expect from the server.
    HelloReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    grpc::Status status = stub_->sayHello(&context, request, &reply);
    // Act upon its status.
      // Act upon its status.
    if (status.ok()) {
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
    // Process reply;
  }

 private:
  std::unique_ptr<Query::Stub> stub_;
};

int main() {
  int ret = 0;
  std::string target_str = "localhost:50051";
  PirQuery pirclient(grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));

  // generate query;
#ifdef DEBUG
  std::string user("world");
  std::string hello_reply = pirclient.SayHello(user);
  std::cout << "Greeter received: " << hello_reply << std::endl;
#endif

  const size_t desired_index = 5;
  const vector<size_t> indices = {desired_index};
  std::vector<size_t> desired_indices = {desired_index};

  //self create reply;
  PIRServerImpl impl(10);
  impl.SetUp();
  auto req_proto = impl.client_->CreateRequest(indices);
  if (req_proto.ok()) {
     std::cout << "req size = " << req_proto->query_size() << std::endl;
  }

  pir::Response reply = pirclient.SendQuery(*req_proto);

  impl.SetUp();
  auto res = impl.client_->ProcessResponse(desired_indices, reply);
   
  if (res.ok()) {
    std::cout << "finish ok" << std::endl;
    std::cout << "size = " << res.value()[0].data() << std::endl;
  }else {
    std::cout << "error in ProcessResponse " << res.status() << std::endl;
  }
  return 0;
}