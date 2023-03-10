
#include "common.h"
#include "echo.grpc.pb.h"
#include "echo.pb.h"
#include "timer.hpp"
#include <grpcpp/support/status.h>
namespace grpc {

struct RateLimitStorage {
  uint64_t num;
  uint64_t period;
  uint64_t cur;
  Timer timer;
  std::mutex mu;
  void init() {
    cur = 0;
    timer.set();
  }
  bool limit() {
    mu.lock();
    if (timer.diff_s().count() > period) {
      timer.set();
      cur = 0;
    }
    bool ret = cur <= num;
    ++cur;
    mu.unlock();
    return ret;
  }
};

class RateLimitInterceptor : public experimental::Interceptor {
public:
  static const string name;
  explicit RateLimitInterceptor(
      experimental::ClientRpcInfo *info, std::shared_ptr<RateLimitStorage> storage) {
        _storage = storage;
      }

  void Intercept(experimental::InterceptorBatchMethods *methods) override {
    bool hijack = false;
    if (methods->QueryInterceptionHookPoint(
            experimental::InterceptionHookPoints::PRE_SEND_INITIAL_METADATA)) {
      LOG(INFO) << name << ":PRE_SEND_INITIAL_METADATA";
      if (!_storage->limit()) {
        LOG(WARNING) << "hijacked";
        hijack = true;
        _limited = true;
      } else {
        LOG(INFO) << "pass";
      } 
    }
    if (methods->QueryInterceptionHookPoint(
            experimental::InterceptionHookPoints::PRE_SEND_MESSAGE)) {
      // LOG(INFO) << name << ":PRE_SEND_MESSAGE";
    
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
      if (_limited) {
        LOG(INFO) << name << ":POST_RECV_STATUS";
        auto status = methods->GetRecvStatus();
        *status = grpc::Status(grpc::StatusCode::RESOURCE_EXHAUSTED, "Rejected by ratelimiter");
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
  std::shared_ptr<RateLimitStorage> _storage;
  bool _limited = false;
};

const string RateLimitInterceptor::name = "ratelimiter";

class RateLimitInterceptorFactory
    : public experimental::ClientInterceptorFactoryInterface {
public:
  RateLimitInterceptorFactory() {
    _storage = std::make_shared<RateLimitStorage>();
    _storage->num = 5;
    _storage->period = 60;
    _storage->init();
  }

  experimental::Interceptor *
  CreateClientInterceptor(experimental::ClientRpcInfo *info) override {
    return new RateLimitInterceptor(info, _storage);
  }
private:
  std::shared_ptr<RateLimitStorage> _storage;
};

extern "C" {
experimental::ClientInterceptorFactoryInterface *
CreateClientInterceptorFactory() {
  return new RateLimitInterceptorFactory();
}
}
} // namespace grpc