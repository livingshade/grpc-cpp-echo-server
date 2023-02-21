#include <stdlib.h>
#include <thread>
#include <time.h>
#include <unistd.h>
#include <vector>
#include <atomic>
#include <functional>
#include <thread>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <assert.h>
#include <dlfcn.h>

void background_thread(void *addr) {
    int server_fd;
    int accept_fd;
    int opt = 1;
    sockaddr_in address;
    int addrlen = sizeof(address);
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        cerr << "sucket failed" << endl;
        exit(-1);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        cerr << "set socker failed" << endl;
        exit(-1);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0) {
        cerr << "bind failed" << endl;
        exit(-1);
    }

    if (listen(server_fd, 1) < 0) {
        cerr << "listen failed" << endl;
        exit(-1);
    }

    if ((accept_fd = accept(server_fd, (sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        cerr << "accept failed" << endl;
        exit(-1);
    }

    char buffer[MAX_SIZE] = {};
    int len = read(accept_fd, buffer, MAX_SIZE);
    int wrote = write_file(SO_NAME_WRITE.c_str(), buffer, len);
    assert(len == wrote);

    auto load_fp = (fn_t)load_symbol(SO_NAME_WRITE.c_str(), "add");
    fp = load_fp;
    cout << "changed to loaded fp!" << endl;
    send(accept_fd, "ack", 3, 0);

    auto gen_fp = (gen_fn_t)load_symbol(SO_NAME_WRITE.c_str(), "gen");
    auto ret = gen_fp();
    cout << "val = " << ret->get_value() << endl;
    delete ret;
    close(accept_fd);
    shutdown(server_fd, SHUT_RDWR);
    return;
}