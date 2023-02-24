
#include "common.h"
#include "echo.grpc.pb.h"
#include "echo.pb.h"

namespace grpc {

class NaiveServerInterceptor : public experimental::Interceptor {
public:
    explicit NaiveServerInterceptor(experimental::ServerRpcInfo * /*info*/) {}

    void Intercept(experimental::InterceptorBatchMethods *methods) override {
        if (methods->QueryInterceptionHookPoint(experimental::InterceptionHookPoints::PRE_SEND_MESSAGE)) {
            string old_msg = static_cast<const pb::Msg *>(methods->GetSendMessage())->body();
            LOG(INFO) << "PRE_SEND_MESSAGE " << old_msg;
            new_msg_.set_body("Good bye:" + old_msg);
            methods->ModifySendMessage(&new_msg_);
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