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

const int MAX_SIZE = 1e5;
const int PORT = 8080;
const string SO_NAME_WRITE = "libadd2.so";
const string SO_NAME_READ = "libadd.so";


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

int write_file(string name, void *addr, int len) {
    system(("rm -f " + name).c_str());
    // otherwise we get permisson denied, idk why
    int fd = open(name.c_str(), O_CREAT | O_RDWR | O_TRUNC, S_IRWXU);
    if (fd < 0) {
        cout << "fd error " << strerror(errno) << endl;
        exit(-1);
    }
    int wrote = write(fd, addr, len);
    fsync(fd);
    cout << "wrote " << wrote << " bytes, expect: " << len << endl;
    return wrote;
}

void* load_symbol(string file, string symbol) {
    // note that ./ is necessary, otherwise the it won't be interpreted as path
    void* handle = dlopen(("./" + file).c_str(), RTLD_NOW);
    if (!handle) {
        cerr << "error on dlopen " << dlerror() << endl;
        exit(-1);
    }
    dlerror();
    void* fp = dlsym(handle, symbol.c_str());
    auto err = dlerror(); 
    if (err) {
        cerr << "error on dlsym " << dlerror() << endl;
        exit(-1);
    }
    return fp;
}

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

    close(accept_fd);
    shutdown(server_fd, SHUT_RDWR);
    return;
}


