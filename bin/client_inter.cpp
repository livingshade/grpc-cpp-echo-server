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

#include <grpcpp/grpcpp.h>
#include <grpcpp/security/server_credentials.h>

#include <iostream>
#include <memory>
#include <string>

#include "common.h"
#include "config.h"
#include "echo.grpc.pb.h"
#include "echo.pb.h"
#include "interceptors_util.h"
#include "share.h"
using namespace grpc;
using pb::EchoService;
using pb::Msg;
using namespace std;
using factory_fp = experimental::ClientInterceptorFactoryInterface *(*)();

class EchoClient {
public:
    EchoClient(std::shared_ptr<Channel> channel) : stub_(EchoService::NewStub(channel)) {}

    // Assembles the client's payload, sends it and presents the response back
    // from the server.
    std::string SayHello(const std::string &user) {
        // Data we are sending to the server.
        Msg request;
        request.set_body(user);

        // Container for the data we expect from the server.
        Msg reply;

        // Context for the client. It could be used to convey extra information
        // to the server and/or tweak certain RPC behaviors.
        ClientContext context;

        // The actual RPC.
        Status status = stub_->echo(&context, request, &reply);

        // Act upon its status.
        if (status.ok()) {
            return reply.body();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return "RPC failed";
        }
    }

private:
    std::unique_ptr<EchoService::Stub> stub_;
};

int main(int argc, char **argv) {
    std::string server_address = SERVER_ADDRESS + ":" + std::to_string(SERVER_PORT);

    // auto channel = grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials());
    // std::cout << "target addr: " << target_str << std::endl;
    // EchoClient client(channel);
    // std::string user("world");
    // while (1) {
    //     std::string reply = client.SayHello(user);
    //     std::cout << "Client received: " << reply << " (ctrl+c to exit)" << std::endl;
    //     sleep(5);
    // }

    vector<unique_ptr<experimental::ClientInterceptorFactoryInterface>> creators;

    void *handle = nullptr;
    auto fp =
        (factory_fp)(load_symbol("./so/librate_limiter.so", SYMBOL_NAME_CLIENT, handle));

    unique_ptr<experimental::ClientInterceptorFactoryInterface> ptr(fp());
    creators.emplace_back(std::move(ptr));

    ChannelArguments args;
    auto channel = experimental::CreateCustomChannelWithInterceptors(server_address, grpc::InsecureChannelCredentials(),
                                                                     args, std::move(creators));

    std::cout << "server addr: " << server_address << std::endl;
    EchoClient client(channel);
    std::string user("world");
    while (1) {
        std::string reply = client.SayHello(user);
        std::cout << "Client received:[" << reply << "] (ctrl+c to exit)" << std::endl;
        sleep(1);
    }
    return 0;
}
