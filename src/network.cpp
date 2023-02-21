#include "network.h"
using namespace std;
int create_server_and_listen(int port, int &server_fd, int &accept_fd)
{
    int opt = 1;
    sockaddr_in address;
    int addrlen = sizeof(address);
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        LOG(ERROR) << "socket failed " << strerror(errno);
        return -1;
    }
    if (setsockopt(server_fd,
                   SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT,
                   &opt,
                   sizeof(opt)))
    {
        LOG(ERROR) << "set socket failed " << strerror(errno);
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (sockaddr *) &address, sizeof(address)) < 0)
    {
        LOG(ERROR) << "bind failed " << strerror(errno);
        return -1;
    }

    if (listen(server_fd, 1) < 0)
    {
        LOG(ERROR) << "listen failed " << strerror(errno);
        return -1;
    }

    if ((accept_fd = accept(
             server_fd, (sockaddr *) &address, (socklen_t *) &addrlen)) < 0)
    {
        LOG(ERROR) << "accept failed " << strerror(errno);
        return -1;
    }
    return 0;
}

int close_server(int server_fd, int accept_fd)
{
    close(accept_fd);
    shutdown(server_fd, SHUT_RDWR);
    return 0;
}

int connect_client(const string &addr,
                   int port,
                   int &client_fd,
                   int &connect_fd)
{
    sockaddr_in address;

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        LOG(ERROR) << "socket failed " << strerror(errno);
        return -1;
    }
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (inet_pton(AF_INET, addr.c_str(), &address.sin_addr) <= 0)
    {
        LOG(ERROR) << "addr translate failed " << strerror(errno);
        return -1;
    }

    if ((connect_fd =
             connect(client_fd, (sockaddr *) &address, sizeof(address))) < 0)
    {
        LOG(ERROR) << "connect failed " << strerror(errno);
        return -1;
    }

    return 0;
}