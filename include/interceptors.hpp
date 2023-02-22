#include <fcntl.h>

#include <memory>
#include <vector>

#include "common.h"
#include "echo.grpc.pb.h"
#include "echo.pb.h"

namespace grpc {
class NaiveInterceptor : public experimental::Interceptor {
public:
    explicit NaiveInterceptor(experimental::ClientRpcInfo *info) {
        info_ = info;
        // Make sure it is the right method
    }

    void Intercept(experimental::InterceptorBatchMethods *methods) override {
        bool hijack = false;
        if (methods->QueryInterceptionHookPoint(experimental::InterceptionHookPoints::PRE_SEND_INITIAL_METADATA)) {
            hijack = true;
        }
        if (methods->QueryInterceptionHookPoint(experimental::InterceptionHookPoints::PRE_SEND_MESSAGE)) {
            pb::Msg req;
            auto *buffer = methods->GetSerializedSendMessage();
            auto copied_buffer = *buffer;
            SerializationTraits<pb::Msg>::Deserialize(&copied_buffer, &req).ok();
            LOG(INFO) << "req.body() at PRE_SEND_MESSAGE " << req.body();
        }
        if (methods->QueryInterceptionHookPoint(experimental::InterceptionHookPoints::PRE_SEND_CLOSE)) {}
        if (methods->QueryInterceptionHookPoint(experimental::InterceptionHookPoints::POST_RECV_INITIAL_METADATA)) {}
        if (methods->QueryInterceptionHookPoint(experimental::InterceptionHookPoints::POST_RECV_MESSAGE)) {
            pb::Msg *resp = static_cast<pb::Msg *>(methods->GetRecvMessage());
            LOG(INFO) << "resp.body() at POST_RECV_MESSAGE " << resp->body();
            resp->set_body("Hello intercepted");
        }
        if (methods->QueryInterceptionHookPoint(experimental::InterceptionHookPoints::POST_RECV_STATUS)) {}
        if (methods->QueryInterceptionHookPoint(experimental::InterceptionHookPoints::PRE_RECV_INITIAL_METADATA)) {}
        if (methods->QueryInterceptionHookPoint(experimental::InterceptionHookPoints::PRE_RECV_MESSAGE)) {}
        if (methods->QueryInterceptionHookPoint(experimental::InterceptionHookPoints::PRE_RECV_STATUS)) {}
        if (hijack) {
            methods->Hijack();
        } else {
            methods->Proceed();
        }
    }

private:
    experimental::ClientRpcInfo *info_;
};

class NaiveInterceptorFactory : public experimental::ClientInterceptorFactoryInterface {
public:
    experimental::Interceptor *CreateClientInterceptor(experimental::ClientRpcInfo *info) override {
        return new NaiveInterceptor(info);
    }
};

experimental::ClientInterceptorFactoryInterface *CreateClientInterceptorFactory() {
    return new NaiveInterceptorFactory();
}
}  // namespace grpc