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

#include "config.h"
#include "echo.grpc.pb.h"
#include "echo.pb.h"
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using pb::EchoService;
using pb::Msg;

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
    // Instantiate the client. It requires a channel, out of which the actual
    // RPCs are created. This channel models a connection to an endpoint
    // specified by the argument "--target=" which is the only expected
    // argument. We indicate that the channel isn't authenticated (use of
    // InsecureChannelCredentials()).
    std::string target_str = SERVER_ADDRESS + ":" + std::to_string(SERVER_PORT);

    auto channel = grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials());
    std::cout << "target addr: " << target_str << std::endl;
    EchoClient client(channel);
    std::string user("world");
    while (1) {
        sleep(10);
        std::string reply = client.SayHello(user);
        std::cout << "Client received: " << reply << " ctrl+c to exit" << std::endl;
    }
    return 0;
}
