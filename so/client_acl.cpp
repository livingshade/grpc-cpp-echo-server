
#include "common.h"
#include "echo.grpc.pb.h"
#include "echo.pb.h"
#include "timer.hpp"
#include <grpcpp/support/status.h>
namespace grpc {

class ContentBasedACLInterceptor : public experimental::Interceptor {
public:
  static const string name;
  explicit ContentBasedACLInterceptor(
      experimental::ClientRpcInfo *info, const std::string &content) {
        _content = content;
      }

  void Intercept(experimental::InterceptorBatchMethods *methods) override {
    bool hijack = false;
    if (methods->QueryInterceptionHookPoint(
            experimental::InterceptionHookPoints::PRE_SEND_INITIAL_METADATA)) {
    }
    if (methods->QueryInterceptionHookPoint(
            experimental::InterceptionHookPoints::PRE_SEND_MESSAGE)) {
      LOG(INFO) << name << ":PRE_SEND_MESSAGE";
      auto msg = (pb::Msg*)methods->GetSendMessage();
      if (msg->body().find(_content) != std::string::npos) {
        _rejected = true;
        hijack = true;
        LOG(WARNING) << "rejected msg:" << msg->body();
      } else {
        LOG(INFO) << "passed";
      }
    }
    if (methods->QueryInterceptionHookPoint(
            experimental::InterceptionHookPoints::PRE_SEND_CLOSE)) {
  //    LOG(INFO) << "PRE_SEND_CLOSE";
    }
    if (methods->QueryInterceptionHookPoint(
            experimental::InterceptionHookPoints::POST_RECV_INITIAL_METADATA)) {
  //     LOG(INFO) << "POST_RECV_INITIAL_METADATA";
    }
    if (methods->QueryInterceptionHookPoint(
            experimental::InterceptionHookPoints::POST_RECV_MESSAGE)) {
  //    LOG(INFO) << name << ":POST_RECV_MESSAGE";

    }
    if (methods->QueryInterceptionHookPoint(
            experimental::InterceptionHookPoints::POST_RECV_STATUS)) {
      if (_rejected) {
        LOG(INFO) << name << ":POST_RECV_STATUS";
        auto status = methods->GetRecvStatus();
        *status = grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Rejected by content based ACL");
      }
    }
    if (methods->QueryInterceptionHookPoint(
            experimental::InterceptionHookPoints::PRE_RECV_INITIAL_METADATA)) {
  //     LOG(INFO) << "PRE_RECV_INITIAL_METADATA";
    }
    if (methods->QueryInterceptionHookPoint(
            experimental::InterceptionHookPoints::PRE_RECV_MESSAGE)) {
  //     LOG(INFO) << "PRE_RECV_MESSAGE";
    }
    if (methods->QueryInterceptionHookPoint(
            experimental::InterceptionHookPoints::PRE_RECV_STATUS)) {
  //   LOG(INFO) << "PRE_RECV_STATUS";
    }
    if (hijack) {
      methods->Hijack();
    } else {
      methods->Proceed();
    }
  }

private:
  std::string _content;
  bool _rejected = false;
};

const string ContentBasedACLInterceptor::name = "content-based ACL";

class ContentBasedACLInterceptorFactory
    : public experimental::ClientInterceptorFactoryInterface {
public:
  ContentBasedACLInterceptorFactory(const std::string &content) {
    _content = content;
  }
  ContentBasedACLInterceptorFactory() {
    _content = "Alice";
  }
  experimental::Interceptor *
  CreateClientInterceptor(experimental::ClientRpcInfo *info) override {
    return new ContentBasedACLInterceptor(info, _content);
  }
private:
  std::string _content;
};

extern "C" {
experimental::ClientInterceptorFactoryInterface *
CreateClientInterceptorFactory() {
  return new ContentBasedACLInterceptorFactory();
}
}
} // namespace grpc