
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

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
