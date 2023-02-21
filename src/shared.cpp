#include <assert.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include <atomic>
#include <fstream>
#include <functional>
#include <iostream>
#include <thread>
#include <vector>

using namespace std;

const int MAX_SIZE = 100000;
const int hah = 0;
int read_file(string file, void *addr) {
    ifstream fd(file.c_str(), ifstream::binary);
    if (!fd) {
        cerr << "open file error" << endl;
        return -1;
    }
    fd.read((char *)addr, MAX_SIZE);
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

void *load_symbol(string file, string symbol) {
    // note that ./ is necessary, otherwise the it won't be interpreted as path
    void *handle = dlopen(("./" + file).c_str(), RTLD_NOW);
    if (!handle) {
        cerr << "error on dlopen " << dlerror() << endl;
        exit(-1);
    }
    dlerror();
    void *fp = dlsym(handle, symbol.c_str());
    auto err = dlerror();
    if (err) {
        cerr << "error on dlsym " << dlerror() << endl;
        exit(-1);
    }
    return fp;
}