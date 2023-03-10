#pragma once
#include <string>

const int SERVER_PORT = 8081;
const int NF_PORT = 12345;
const int FILE_MAX_SIZE = 1e8;
const int SO_SIZE = 8752800;
const std::string SERVER_ADDRESS = "127.0.0.1";
const std::string SO_NAME_CLIENT = "./so/libclient_acl.so";
const std::string SO_NAME_SERVER = "./so/libnaive_server_interceptor.so";
const std::string SYMBOL_NAME_CLIENT = "CreateClientInterceptorFactory";
const std::string SYMBOL_NAME_SERVER = "CreateServerInterceptorFactory";
