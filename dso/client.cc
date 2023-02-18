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
#include <arpa/inet.h>
#include <dlfcn.h>


using namespace std;

using fn_t = int(*)(int, int);

fn_t fp = nullptr;

int sub(int x, int y) {
    return x - y;
}

int mul(int x, int y) {
    return x * y;
}

const int PORT = 8080;
const int MAX_SIZE = 1e5;

// todo replace stream
int read_file(string file, void *addr) {
    ifstream fd(file.c_str(), ifstream::binary);
    if (!fd) {
        cerr << "open file error" << endl;
        return -1;
    }
    fd.read((char*)addr, MAX_SIZE);
    int len = fd.gcount();
    cout << "read " << len << " byte(s)" << endl;
    fd.close();
    return len;
}

int main() {
    int client_fd;
    int connect_fd;
    int opt = 1;
    sockaddr_in address;
    int addrlen = sizeof(address);
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        cerr << "sucket failed" << endl;
        exit(-1);
    }
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0) {
        cerr << "addr failed" << endl;
        exit(-1);
    }

    if ((connect_fd = connect(client_fd, (sockaddr*)&address, sizeof(address))) < 0) {
         cerr << "connect failed" << endl;
        exit(-1);
    }


    char *buffer = (char*)malloc(MAX_SIZE);
    int len = read_file("libadd.so", buffer);
    send(client_fd, buffer, len, 0);
    cout << "sent libadd.so" << endl;
    close(connect_fd);
    return 0;
}


