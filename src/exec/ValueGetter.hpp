
// Copyright 2023 Schwarzthal Tech
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <string>
#include <vector>

#include <vendor/redismodule.h>

#include "util/util.hpp"

using RedisHash = std::map<std::string, ByteArray>;

class ValueGetter {
	RedisModuleCtx* ctx;

public:
	explicit ValueGetter(RedisModuleCtx* ctx);

	[[nodiscard]] auto readString(RedisModuleString* keyName) const -> std::string;

	[[nodiscard]] auto readHashKey(RedisModuleString* keyName) const -> RedisHash;

	[[nodiscard]] static auto readHashReply(RedisModuleCallReply* reply) -> RedisHash;

	[[nodiscard]] static auto readStringReply(RedisModuleCallReply* reply) -> std::string;

	[[nodiscard]] static auto readBytesReply(RedisModuleCallReply* reply) -> ByteArray;

	[[nodiscard]] static auto readIntegerReply(RedisModuleCallReply* reply) -> long long;

private:
	auto getKeyType(RedisModuleString* keyName) const -> int;
};
