#include <grpcpp/server_builder.h>

#include "common.h"
#include "config.h"
#include "echo.grpc.pb.h"
#include "echo.pb.h"
#include "grpchdr.h"
#include "interceptors_util.h"
#include "network.h"
#include "share.h"
using namespace std;
using namespace grpc;
using namespace pb;

using s_factory_fp = experimental::ServerInterceptorFactoryInterface *(*)();

class EchoServiceImpl final : public EchoService::Service {
    Status echo(ServerContext *context, const Msg *request, Msg *reply) override {
        LOG(INFO) << "EchoServiceImpl::echo";
        std::string prefix("Hello!");
        reply->set_body(prefix + request->body());
        return Status::OK;
    }
};

void thread_function(int port, const std::string filename, void *ptr) {
    int server_fd, accept_fd;
    LOG(INFO) << "NF thread entered";
    CHECK(create_server_and_listen(port, server_fd, accept_fd) == 0);
    auto buffer = new char[FILE_MAX_SIZE];
    auto p = buffer;
    int len = -1, total = 0;
    sleep(1);
    // while (total < SO_SIZE && (len = read(accept_fd, p, FILE_MAX_SIZE)) > 0) {
    //     p = p + len;
    //     total += len;
    //     LOG(INFO) << "read " << len << "total " << total;
    // }
    // LOG(INFO) << "Finish read, total = " << total;
    // CHECK(total >= 0);
    // write_file(filename, buffer, total);
    // LOG(INFO) << "Got .so from client, size = " << total;

    // start to change interceptor

    // We pretent we read the so from the function above
    // TODO change to gRPC
    auto server_ptr = (grpc::Server *)ptr;
    vector<unique_ptr<experimental::ServerInterceptorFactoryInterface>> creators;

    void *handle = nullptr;
    auto fp = (s_factory_fp)(load_symbol(SO_NAME_SERVER, "CreateServerInterceptorFactory", handle));

    LOG(INFO) << "Symbol Loaded From " << SO_NAME_SERVER;
    unique_ptr<experimental::ServerInterceptorFactoryInterface> factory(fp());
    creators.emplace_back(std::move(factory));

    // actual change begins here
    LOG(INFO) << "Start change";
    server_ptr->SetInterceptorCreators(std::move(creators));
    LOG(INFO) << "End Change";

    close_server(server_fd, accept_fd);
}

int main(int argc, char *argv[]) {
    // google::InitGoogleLogging(argv[0]);
    auto server_address = SERVER_ADDRESS + ":" + std::to_string(SERVER_PORT);

    EchoServiceImpl service;
    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();

    auto builder = new ServerBuilder;
    builder->AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder->RegisterService(&service);

    std::unique_ptr<Server> server(builder->BuildAndStart());
    LOG(INFO) << "Server Built and Started";

    auto raw = server.get();

    auto nf_receiver = std::thread(thread_function, NF_PORT, SO_NAME_SERVER, server.get());

    auto server_runner = std::thread([&raw, &server_address] {
        std::cout << "Server listening on " << server_address << std::endl;
        LOG(INFO) << "Server thread waiting";
        raw->Wait();
        LOG(INFO) << "Server thread exit";
    });
    int sec = 0;
    while (true) {
        LOG(INFO) << "waiting sec = " << sec;
        sleep(10);
        sec += 10;
    }
    return 0;
}
