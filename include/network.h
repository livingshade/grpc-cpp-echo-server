#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "common.h"

// 0 on success, -1 on error, server_fd for write, accept_fd for read
int create_server_and_listen(int port, int &server_fd, int &accept_fd);

int close_server(int server_fd, int accept_fd);

// 0 on success, -1 on error, client_fd for write, connect_fd for read
int connect_client(const std::string &addr, int port, int &client_fd, int &connect_fd);