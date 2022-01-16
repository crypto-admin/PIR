#include "pir/cpp/client.h"
#include "seal/seal.h"

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
using namespace pir; 


class PirQuery {
 public:
  PirQuery(std::shared_ptr<Channel> channel)
      : stub_(Query::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  std::string SendPubkeyA(const pir::Request& query) {
    // Data we are sending to the server.
    // Request request;
    // Ciphertexts* ct;
    // ct = request.add_query();
    // std::cout << "test1" << std::endl;
    // // ct->set_ct(0, query);
    // ct->add_ct(query);
    
    std::cout << "test2" << std::endl;


    // Container for the data we expect from the server.
    Response reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    stub_->sendQuery(&context, query, &reply);
  }

 private:
  std::unique_ptr<Query::Stub> stub_;
};

int main() {
  printf("sender starting..");
  int ret = 0;

  std::string target_str = "localhost:50051";
  PirQuery pirclient(grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
  std::string pubkeyA("12345688888888889999999");
  // generate query;

  EncryptionParameters encryption_params_;
  constexpr uint32_t POLY_MODULUS_DEGREE = 4096;
  encryption_params_ = GenerateEncryptionParams(POLY_MODULUS_DEGREE, 16);
  int dbsize = 100;
  int elem_size = 16;
  int dimensions = 1;
  shared_ptr<PIRParameters> pir_params_ =
        *(CreatePIRParameters(dbsize, elem_size, dimensions, encryption_params_,
                              false));
  std::unique_ptr<PIRClient> client_ = *(PIRClient::Create(pir_params_));

  const size_t desired_index = 5;
  const vector<size_t> indices = {desired_index};

  auto req_proto = client_->CreateRequest(indices);
  if (req_proto.ok()) {
    req_proto->query_size();
  }
  std::string res;
  // std::cout << "ok = " << req_proto.value().SerializePartialToString(&res) << std::endl;
  // std::cout << "res t= " << res << std::endl;
  std::string reply = pirclient.SendPubkeyA((*req_proto));
  // std::cout << "ot12 pubA reply = " <<  reply  << std::endl;

  printf("have sendt pubkey to client.\n");

  return 0;
}