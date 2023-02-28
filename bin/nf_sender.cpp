#include <functional>

#include "common.h"
#include "config.h"
#include "echo.grpc.pb.h"
#include "echo.pb.h"
#include "grpchdr.h"
#include "interceptors_util.h"
#include "network.h"
#include "share.h"

using namespace std;

int main(int argc, char *argv[]) {
    int client_fd;
    int connect_fd;
    // TODO add parse argv
    connect_client(SERVER_ADDRESS, NF_PORT, client_fd, connect_fd);
    auto buffer = SO_NAME_SERVER.c_str();
    LOG(INFO) << "SO name is " << buffer;
    auto wrote = write(client_fd, buffer, SO_NAME_SERVER.size());
    LOG(INFO) << "Sender sent .so name" << wrote;
    sleep(1);
    return 0;
}