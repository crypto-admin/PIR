#include "pir/cpp/client.h"
#include "seal/seal.h"
// #include "pir/cpp/server_impl.h"
#include "pir/cpp/client_impl.h"

// add by byte
#include "pir/proto/payload.grpc.pb.h"
#include <grpcpp/grpcpp.h>

#include "parameters.h"
#include <fstream>
#include <string>

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


// struct pirparams {
//   uint32_t poly_modulus_degree;
//   uint32_t ele_size;
//   uint8_t dimensions;
//   uint32_t plain_mod_bit_size;
//   bool use_ciphertext_multiplication;
//   uint64_t db_size;
// };
pirparams onlineparam = {4096, 128, 1, 20, true, 1000000}; 
uint8_t param_size = 6;

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

// read server params from config file, as csv, json..
int parserparam() {
  string config;
  ifstream config_file("client_config.csv", ios::in);
  if (!config_file) {
    std::cout << "open config file fail." << std::endl;
    return 1;
  }
  uint32_t param_temp[param_size]; // 7 is para num of pirparams
  int index = 0;
  while (getline(config_file, config)) {
    param_temp[index] = atoi(config.c_str());
    index++;
    if (index > param_size) break;
  }
  if (index == param_size) {
    onlineparam.poly_modulus_degree = param_temp[0];
    onlineparam.ele_size = param_temp[1];
    onlineparam.dimensions = param_temp[2];
    onlineparam.plain_mod_bit_size = param_temp[3];
    onlineparam.use_ciphertext_multiplication = param_temp[4]; // int to bool;
    onlineparam.db_size = param_temp[5];
  } else {
    return 2; // param size error;
  }
  
  return 0;
}

int main() {
  int ret = 0;
  std::string target_str = "localhost:50051";
  PirQuery pirclient(grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));

  int config_res = parserparam();
  if (config_res != 0) {
    std::cout << "read config file error, error code = " << config_res << std::endl;
  }

  // generate query;
#ifdef DEBUG
  std::string user("world");
  std::string hello_reply = pirclient.SayHello(user);
  std::cout << "Greeter received: " << hello_reply << std::endl;
#endif

  const size_t desired_index = 5;
  const vector<size_t> indices = {desired_index};
  std::vector<size_t> desired_indices = {desired_index};

  PIRClientImpl impl(onlineparam);
  impl.SetUp();
  auto req_proto = impl.client_->CreateRequest(indices);
  if (req_proto.ok()) {
     std::cout << "client generate request, size = " << req_proto->query_size() << std::endl;
  } else {
    std::cout << "client generate request fail." << std::endl;
  }

  pir::Response reply = pirclient.SendQuery(*req_proto);

  // impl.SetUp();
  auto res = impl.client_->ProcessResponse(desired_indices, reply);
   
  if (res.ok()) {
    std::cout << "client finish query, get the following data: " << std::endl;
    std::cout << (*res)[0].substr(0, 10) << std::endl;
  }else {
    std::cout << "error in ProcessResponse " << res.status() << std::endl;
  }
  return 0;
}