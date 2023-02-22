#include <functional>

#include "common.h"
#include "echo.grpc.pb.h"
#include "echo.pb.h"
#include "grpchdr.h"
#include "interceptors_util.h"
#include "share.h"
using namespace std;
using namespace grpc;
using namespace pb;

const int PORT = 8081;
const string SO_NAME = "./so/libnaive_interceptor.so";

using factory_fp = experimental::ClientInterceptorFactoryInterface *(*)();

class EchoServiceImpl final : public EchoService::Service {
    Status echo(ServerContext *context, const Msg *request, Msg *reply) override {
        std::string prefix("Hello!");
        reply->set_body(prefix + request->body());
        return Status::OK;
    }
};

int main() {
    ServerBuilder builder;
    auto server_address = "localhost:" + std::to_string(PORT);
    builder.AddListeningPort(server_address, InsecureServerCredentials());
    EchoServiceImpl service;

    builder.RegisterService(&service);
    auto server = builder.BuildAndStart();

    vector<unique_ptr<experimental::ClientInterceptorFactoryInterface>> creators;

    void *handle = nullptr;
    auto fp = (factory_fp)(load_symbol(SO_NAME, "CreateClientInterceptorFactory", handle));

    unique_ptr<experimental::ClientInterceptorFactoryInterface> ptr(fp());
    creators.emplace_back(std::move(ptr));

    ChannelArguments args;
    auto channel =
        experimental::CreateCustomChannelWithInterceptors(server_address, nullptr, args, std::move(creators));

    MakeCall(channel);
    return 0;
}