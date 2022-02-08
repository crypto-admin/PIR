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


// Logic and data behind the server's behavior.
class GreeterServiceImpl final : public Query::Service {
  grpc::Status sendQuery(ServerContext* context, const Request* request,
                 Response* reply) override {
    PIRServerImpl impl(10);
    impl.SetUp();
    const vector<size_t> indices = {5};
    auto req_proto = impl.client_->CreateRequest(indices);

    auto test_reply = impl.server_->ProcessRequest(*request);
    // auto test_reply = impl.server_->ProcessRequest(*req_proto);
    std::cout << "server ProcessRequest finish." << test_reply.ok() << std::endl;
    
    *reply = test_reply.value();
    std::vector<size_t> desired_indices = {5};                                            
    auto res = impl.client_->ProcessResponse(desired_indices, *test_reply);
    
    if (res.ok()) {
      std::cout << "finish ok" << std::endl;
      std::cout << "size = " << (*res)[0].c_str() << std::endl;
    }else {
      std::cout << "error in ProcessResponse " << res.status() << std::endl;
    }
    return grpc::Status::OK;
  }
  // Hello imple just for test.
  grpc::Status sayHello(ServerContext* context, const HelloRequest* request,
                  HelloReply* reply) override {
    std::string prefix("Hello ");
    reply->set_message(prefix + request->name());

    return grpc::Status::OK;
  }
};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  GreeterServiceImpl service;

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

int main(int argc, char** argv) {
  RunServer();

  return 0;
}