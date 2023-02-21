#include <dlfcn.h>
#include <fcntl.h>

#include "common.h"

int read_file(const std::string &filename, void *addr, int len);

int write_file(const std::string &filename, void *addr, int len);

void *load_symbol(const std::string &file, const std::string &symbol, void *&handle);

void close_sym_handle(void *handle);