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

#include "ValueGetter.hpp"

ValueGetter::ValueGetter(RedisModuleCtx* ctx) : ctx(ctx) {
}

auto ValueGetter::readString(RedisModuleString* keyName) const -> std::string {
	auto *const reply = RedisModule_Call(ctx, "GET", "s", keyName);

	if (RedisModule_CallReplyType(reply) != REDISMODULE_REPLY_STRING) {
		return {};
	}

	auto result = readStringReply(reply);

	RedisModule_FreeCallReply(reply);

	return result;
}

auto ValueGetter::readHashKey(RedisModuleString* keyName) const -> RedisHash {
	auto *const reply = RedisModule_Call(ctx, "HGETALL", "s", keyName);

	auto result = readHashReply(reply);

	RedisModule_FreeCallReply(reply);

	return result;
}

auto ValueGetter::readHashReply(RedisModuleCallReply* reply) -> RedisHash {
	auto result = RedisHash();

	if (RedisModule_CallReplyType(reply) != REDISMODULE_REPLY_ARRAY) {
		return {};
	}

	const std::size_t count = RedisModule_CallReplyLength(reply);

	if (count % 2 != 0) {
		return {};
	}

	for (std::size_t i = 0; i < count; i += 2) {
		const auto key = readStringReply(RedisModule_CallReplyArrayElement(reply, i));
		const auto value = readBytesReply(RedisModule_CallReplyArrayElement(reply, i + 1));

		result.insert({key, value});
	}

	return result;
}

auto ValueGetter::readStringReply(RedisModuleCallReply* reply) -> std::string {
	if (reply == nullptr || RedisModule_CallReplyType(reply) != REDISMODULE_REPLY_STRING) {
		return {};
	}

	std::size_t len = 0;
	const auto *const ptr = RedisModule_CallReplyStringPtr(reply, &len);

	return {ptr, len};
}

auto ValueGetter::readBytesReply(RedisModuleCallReply* reply) -> ByteArray {
	auto receiver = ByteArray();

	std::size_t len = 0;
	const auto *const ptr = RedisModule_CallReplyStringPtr(reply, &len);

	for (std::size_t i = 0; i < len; i++) {
		receiver.emplace_back(ptr[i]);
	}

	return receiver;
}

auto ValueGetter::readIntegerReply(RedisModuleCallReply* reply) -> long long {
	if (reply == nullptr || RedisModule_CallReplyType(reply) != REDISMODULE_REPLY_INTEGER) {
		return -1;
	}

	return RedisModule_CallReplyInteger(reply);
}

auto ValueGetter::getKeyType(RedisModuleString* keyName) const -> int {
	auto *const handle = RedisModule_OpenKey(ctx, keyName, REDISMODULE_READ | REDISMODULE_OPEN_KEY_NOEFFECTS);

	const auto keyType = RedisModule_KeyType(handle);

	RedisModule_CloseKey(handle);

	return keyType;
}
