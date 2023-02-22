//
//
// Copyright 2018 gRPC authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//
#include "interceptors_util.h"

#include "echo.grpc.pb.h"
#include "echo.pb.h"

using pb::Msg;

const int kNumStreamingMessages = 10;

namespace grpc {
void MakeCall(const std::shared_ptr<Channel> &channel, const StubOptions &options) {
    auto stub = pb::EchoService::NewStub(channel, options);
    ClientContext ctx;
    Msg req;
    ctx.AddMetadata("testkey", "testvalue");
    req.set_body("Hello");
    Msg resp;
    Status s = stub->echo(&ctx, req, &resp);
}

bool CheckMetadata(const std::multimap<grpc::string_ref, grpc::string_ref> &map, const string &key,
                   const string &value) {
    for (const auto &pair : map) {
        if (pair.first.starts_with(key) && pair.second.starts_with(value)) {
            return true;
        }
    }
    return false;
}

bool CheckMetadata(const std::multimap<std::string, std::string> &map, const string &key, const string &value) {
    for (const auto &pair : map) {
        if (pair.first == key && pair.second == value) {
            return true;
        }
    }
    return false;
}

}  // namespace grpc