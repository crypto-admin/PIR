/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <fstream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/grpcpp.h>

 // add by byte
#include "pir/proto/payload.grpc.pb.h"

#include "pir/cpp/server.h"
#include "pir/cpp/server_impl.h"
#include "seal/seal.h"
#include "pir/cpp/database.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using pir::Request;
using pir::Response;
using pir::Query;
using pir::HelloRequest;
using pir::HelloReply;
using namespace pir; 
using namespace std;


// struct pirparams {
//   uint32_t poly_modulus_degree;
//   uint32_t ele_size;
//   uint8_t dimensions;
//   uint32_t plain_mod_bit_size;
//   bool use_ciphertext_multiplication;
//   uint64_t db_size;
// };
pirparams onlineparam = {4096, 128, 2, 20, true, 100};
uint8_t param_size = 6;

// Logic and data behind the server's behavior.
class PIRServiceImpl final : public Query::Service {
 public:
  PIRServiceImpl(pirparams onlineparam) {
     impl_ = make_unique<PIRServerImpl>(onlineparam);
     impl_->SetUp();
  }
  grpc::Status sendQuery(ServerContext* context, const Request* request,
                 Response* reply) override {
  
    auto test_reply = impl_->server_->ProcessRequest(*request);  
    *reply = test_reply.value();

    // TODO : add log file, to list every process request;
    std::cout << "server ProcessRequest finish." << test_reply.ok() << std::endl;

    return grpc::Status::OK;
  }
  // Hello imple just for test.
  grpc::Status sayHello(ServerContext* context, const HelloRequest* request,
                  HelloReply* reply) override {
    std::string prefix("Hello ");
    reply->set_message(prefix + request->name());

    return grpc::Status::OK;
  }
 private:
  unique_ptr<PIRServerImpl> impl_;
};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  PIRServiceImpl service(onlineparam);

  grpc::EnableDefaultHealthCheckService(true);
  // grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

// read server params from config file, as csv, json..
int parserparam() {
  string config;
  ifstream config_file("server_config.csv", ios::in);
  if (!config_file.is_open()) {
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

int main(int argc, char** argv) {
  int res =  parserparam();
  if (res != 0) {
    std::cout << "read config file error, error code = " << res << std::endl;
  }
  RunServer();
  return 0;
}