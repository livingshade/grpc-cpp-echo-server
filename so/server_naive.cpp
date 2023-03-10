
#include "common.h"
#include "echo.grpc.pb.h"
#include "echo.pb.h"

namespace grpc {

class NaiveServerInterceptor : public experimental::Interceptor {
public:
    explicit NaiveServerInterceptor(experimental::ServerRpcInfo * /*info*/) {}

    void Intercept(experimental::InterceptorBatchMethods *methods) override {
        if (methods->QueryInterceptionHookPoint(experimental::InterceptionHookPoints::PRE_SEND_INITIAL_METADATA)) {
            // hijack = true;
            LOG(INFO) << "PRE_SEND_INITIAL_METADATA";
        }
        if (methods->QueryInterceptionHookPoint(experimental::InterceptionHookPoints::PRE_SEND_MESSAGE)) {
            string old_msg = static_cast<const pb::Msg *>(methods->GetSendMessage())->body();
            new_msg_.set_body("Good bye:" + old_msg);
            methods->ModifySendMessage(&new_msg_);
            LOG(INFO) << "PRE_SEND_MESSAGE";
        }
        if (methods->QueryInterceptionHookPoint(experimental::InterceptionHookPoints::PRE_SEND_CLOSE)) {
            LOG(INFO) << "PRE_SEND_CLOSE";
        }
        if (methods->QueryInterceptionHookPoint(experimental::InterceptionHookPoints::POST_RECV_INITIAL_METADATA)) {
            LOG(INFO) << "POST_RECV_INITIAL_METADATA";
        }
        if (methods->QueryInterceptionHookPoint(experimental::InterceptionHookPoints::POST_RECV_MESSAGE)) {
            pb::Msg *resp = static_cast<pb::Msg *>(methods->GetRecvMessage());
            resp->set_body("Hello intercepted");
            LOG(INFO) << "POST_RECV_MESSAGE";
        }
        if (methods->QueryInterceptionHookPoint(experimental::InterceptionHookPoints::POST_RECV_STATUS)) {
            LOG(INFO) << "POST_RECV_STATUS";
        }
        if (methods->QueryInterceptionHookPoint(experimental::InterceptionHookPoints::PRE_RECV_INITIAL_METADATA)) {
            LOG(INFO) << "PRE_RECV_INITIAL_METADATA";
        }
        if (methods->QueryInterceptionHookPoint(experimental::InterceptionHookPoints::PRE_RECV_MESSAGE)) {
            LOG(INFO) << "PRE_RECV_MESSAGE";
        }
        if (methods->QueryInterceptionHookPoint(experimental::InterceptionHookPoints::PRE_RECV_STATUS)) {
            LOG(INFO) << "PRE_RECV_STATUS";
        }
        methods->Proceed();
    }

private:
    pb::Msg new_msg_;
};

class NaiveServerInterceptorFactory : public experimental::ServerInterceptorFactoryInterface {
public:
    experimental::Interceptor *CreateServerInterceptor(experimental::ServerRpcInfo *info) override {
        return new NaiveServerInterceptor(info);
    }
};

extern "C" {
experimental::ServerInterceptorFactoryInterface *CreateServerInterceptorFactory() {
    return new NaiveServerInterceptorFactory();
}
}
}  // namespace grpc