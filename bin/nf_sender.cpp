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

int main() {
    int client_fd;
    int connect_fd;
    connect_client(SERVER_ADDRESS, NF_PORT, client_fd, connect_fd);
    auto buffer = new char[FILE_MAX_SIZE];
    int len = read_file(SO_NAME_CLIENT, buffer, FILE_MAX_SIZE);
    CHECK(len >= 0);
    LOG(INFO) << "Sender read .so size = " << len;
    auto wrote = write(client_fd, buffer, len);
    LOG(INFO) << "Sender sent .so " << wrote;
    sleep(1);
    return 0;
}