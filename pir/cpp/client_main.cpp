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
  pir::Response SendPubkeyA(const pir::Request& query) {
    // Data we are sending to the server.
    // Request request;
    // Ciphertexts* ct;
    // ct = request.add_query();
    // std::cout << "test1" << std::endl;
    // // ct->set_ct(0, query);
    // ct->add_ct(query);
    
    // whether query is correct;
    std::cout << "sendPubkeyA: " << query.galois_keys().size() << std::endl;

  
    // Container for the data we expect from the server.
    Response reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    stub_->sendQuery(&context, query, &reply);

    return reply;
    //return "OK";
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
  std::cout << "inter main " << std::endl;
  int ret = 0;

  std::string target_str = "localhost:50051";
  PirQuery pirclient(grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));

  // generate query;
  std::string user("world");
  std::string hello_reply = pirclient.SayHello(user);
  std::cout << "Greeter received: " << hello_reply << std::endl;

  EncryptionParameters encryption_params_;
  constexpr uint32_t POLY_MODULUS_DEGREE = 4096;
  encryption_params_ = GenerateEncryptionParams(POLY_MODULUS_DEGREE, 20);
  int dbsize = 10;
  int elem_size = 7680;
  int dimensions = 1;
  shared_ptr<PIRParameters> pir_params_ =
        *(CreatePIRParameters(dbsize, elem_size, dimensions, encryption_params_,
                              true));


  
  std::unique_ptr<PIRClient> client_ = *(PIRClient::Create(pir_params_));

  const size_t desired_index = 5;
  const vector<size_t> indices = {desired_index};
  std::vector<size_t> desired_indices = {desired_index};

  // auto req_proto = client_->CreateRequest(indices);
  // if (req_proto.ok()) {
  //   std::cout << "req size = " << req_proto->query_size() << std::endl;
  // }
  // pir::Response reply = pirclient.SendPubkeyA(*req_proto);
  ///////////////////////////
  //self create reply;
  PIRServerImpl impl = PIRServerImpl(10);
  impl.SetUp();
  auto req_proto = impl.client_->CreateRequest(indices);
  if (req_proto.ok()) {
     std::cout << "req size = " << req_proto->query_size() << std::endl;
  }

  pir::Response reply = pirclient.SendPubkeyA(*req_proto);

  ///////////////////////////

  auto res = client_->ProcessResponse(desired_indices, reply);
   
  if (res.ok()) {std::cout << "finish ok" << std::endl;}
  else std::cout << "erroro " << res.status() << std::endl;

  return 0;
}