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
#pragma once

#include <grpcpp/channel.h>

#include "common.h"
#include "grpchdr.h"

namespace grpc {

void MakeCall(const std::shared_ptr<Channel> &channel, const StubOptions &options = StubOptions());

void MakeClientStreamingCall(const std::shared_ptr<Channel> &channel);

void MakeServerStreamingCall(const std::shared_ptr<Channel> &channel);

void MakeBidiStreamingCall(const std::shared_ptr<Channel> &channel);

void MakeAsyncCQCall(const std::shared_ptr<Channel> &channel);

void MakeAsyncCQClientStreamingCall(const std::shared_ptr<Channel> &channel);

void MakeAsyncCQServerStreamingCall(const std::shared_ptr<Channel> &channel);

void MakeAsyncCQBidiStreamingCall(const std::shared_ptr<Channel> &channel);

void MakeCallbackCall(const std::shared_ptr<Channel> &channel);

bool CheckMetadata(const std::multimap<grpc::string_ref, grpc::string_ref> &map, const string &key,
                   const string &value);

bool CheckMetadata(const std::multimap<std::string, std::string> &map, const string &key, const string &value);
}  // namespace grpc