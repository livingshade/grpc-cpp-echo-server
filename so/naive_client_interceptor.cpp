#include <fcntl.h>

#include "common.h"
#include "echo.grpc.pb.h"
#include "echo.pb.h"
// get struct before send and after recv both client and server
namespace grpc {
class NaiveClientInterceptor : public experimental::Interceptor {
public:
    explicit NaiveClientInterceptor(experimental::ClientRpcInfo *info) {
        info_ = info;
        // Make sure it is the right method
    }

    void Intercept(experimental::InterceptorBatchMethods *methods) override {
        bool hijack = false;
        if (methods->QueryInterceptionHookPoint(experimental::InterceptionHookPoints::PRE_SEND_INITIAL_METADATA)) {
            LOG(INFO) << "PRE_SEND_INITIAL_METADATA";
        }
        if (methods->QueryInterceptionHookPoint(experimental::InterceptionHookPoints::PRE_SEND_MESSAGE)) {
            LOG(INFO) << "PRE_SEND_MESSAGE";

            auto req = (const pb::Msg *)methods->GetSendMessage();
            auto nreq = new pb::Msg;
            nreq->set_body("[PRE_SEND]" + req->body());
            methods->ModifySendMessage(nreq);
        }
        if (methods->QueryInterceptionHookPoint(experimental::InterceptionHookPoints::PRE_SEND_CLOSE)) {
            LOG(INFO) << "PRE_SEND_CLOSE";
        }
        if (methods->QueryInterceptionHookPoint(experimental::InterceptionHookPoints::POST_RECV_INITIAL_METADATA)) {
            LOG(INFO) << "POST_RECV_INITIAL_METADATA";
        }
        if (methods->QueryInterceptionHookPoint(experimental::InterceptionHookPoints::POST_RECV_MESSAGE)) {
            LOG(INFO) << "POST_RECV_MESSAGE";
            auto req = (pb::Msg *)methods->GetRecvMessage();
            req->set_body("[POST_RECV]" + req->body());
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
        if (hijack) {
            methods->Hijack();
        } else {
            methods->Proceed();
        }
    }

private:
    experimental::ClientRpcInfo *info_;
};

class NaiveClientInterceptorFactory : public experimental::ClientInterceptorFactoryInterface {
public:
    experimental::Interceptor *CreateClientInterceptor(experimental::ClientRpcInfo *info) override {
        return new NaiveClientInterceptor(info);
    }
};

extern "C" {
experimental::ClientInterceptorFactoryInterface *CreateClientInterceptorFactory() {
    return new NaiveClientInterceptorFactory();
}
}
}  // namespace grpc