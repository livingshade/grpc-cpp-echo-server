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
using namespace std;

using fn_t = int(*)(int, int);

fn_t fp = nullptr;

int sub(int x, int y) {
    return x - y;
}

int mul(int x, int y) {
    return x * y;
}

void background_thread(void*);

int main() {
    fp = &sub;
    int x = 4, y = 6;
    int ret = fp(x, y);
    cout << "ret: " << ret << endl;
    cout << "start thread" << endl;
    std::thread th(background_thread, &fp);
    for (int i = 0; i < 10; ++i) {
        cout << i << "th seconds: " << fp(x, y) << endl;
        sleep(1);
    }
    th.join();
}


const int PORT = 8080;

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

    char buffer[100] = {};
    int len = read(accept_fd, buffer, 100);
    printf("%s\n", buffer);
    cout << "receive !" << endl;
    //auto ptr = (fn_t)*(uint64_t*)addr;
    //ptr = &mul;
    fp = &mul;
    cout << "changed to mul!" << endl;
    send(accept_fd, "ack", 3, 0);

    close(accept_fd);
    shutdown(server_fd, SHUT_RDWR);
    return;
}


