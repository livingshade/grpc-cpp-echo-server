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

#include "share.h"
using namespace std;

int read_file(const string &filename, void *addr, int len) {
    assert(len >= 0);
    int fd = open(filename.c_str(), O_RDONLY, S_IRWXU);
    if (fd <= 0) {
        LOG(ERROR) << "open failed " << strerror(errno);
        return -1;
    }
    auto read_len = read(fd, addr, len);
    LOG(INFO) << "read " << read_len << " byte(s)";
    close(fd);
    return read_len;
}

int write_file(const string &filename, void *addr, int len) {
    system(("rm -f " + filename).c_str());
    // otherwise we get permisson denied, idk why
    int fd = open(filename.c_str(), O_CREAT | O_RDWR | O_TRUNC, S_IRWXU);
    if (fd < 0) {
        LOG(ERROR) << "fd error " << strerror(errno);
        return -1;
    }
    int wrote = write(fd, addr, len);
    fsync(fd);  // todo remove fsync
    LOG(INFO) << "wrote " << wrote << " bytes, expect: " << len;
    return wrote;
}

void *load_symbol(const string &file, const string &symbol, void *&handle) {
    // note that ./ is necessary, otherwise the it won't be interpreted as path
    handle = dlopen(("./" + file).c_str(), RTLD_NOW);
    if (!handle) {
        LOG(ERROR) << "error on dlopen " << dlerror();
        exit(-1);
    }
    dlerror();
    LOG(INFO) << "try open " << symbol << " in " << file;
    void *fp = dlsym(handle, symbol.c_str());
    auto err = dlerror();
    if (err) {
        LOG(ERROR) << "error on dlsym " << dlerror();
        exit(-1);
    }
    return fp;
}

void close_sym_handle(void *handle) {
    dlclose(handle);
}