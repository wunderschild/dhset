/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ValueGetter.hpp"

ValueGetter::ValueGetter(RedisModuleCtx* ctx) : ctx(ctx) {
}

auto ValueGetter::readString(RedisModuleString* keyName) const -> std::string {
	const auto reply = RedisModule_Call(ctx, "GET", "s", keyName);

	if (RedisModule_CallReplyType(reply) != REDISMODULE_REPLY_STRING) {
		return {};
	}

	auto result = readStringReply(reply);

	RedisModule_FreeCallReply(reply);

	return result;
}

auto ValueGetter::readHashKey(RedisModuleString* keyName) const -> RedisHash {
	const auto reply = RedisModule_Call(ctx, "HGETALL", "s", keyName);

	auto result = readHashReply(reply);

	RedisModule_FreeCallReply(reply);

	return result;
}

auto ValueGetter::readHashReply(RedisModuleCallReply* reply) -> RedisHash {
	auto result = RedisHash();

	if (RedisModule_CallReplyType(reply) != REDISMODULE_REPLY_ARRAY) {
		return {};
	}

	const auto count = RedisModule_CallReplyLength(reply);

	if (count % 2 != 0) {
		return {};
	}

	for (auto i = 0; i < count; i += 2) {
		const auto key = readStringReply(RedisModule_CallReplyArrayElement(reply, i));
		const auto value = readStringReply(RedisModule_CallReplyArrayElement(reply, i + 1));

		result[key] = value;
	}

	return result;
}

auto ValueGetter::readStringReply(RedisModuleCallReply* reply) -> std::string {
	if (reply == nullptr || RedisModule_CallReplyType(reply) != REDISMODULE_REPLY_STRING) {
		return {};
	}

	size_t len;
	const auto ptr = RedisModule_CallReplyStringPtr(reply, &len);

	return {ptr, len};
}

auto ValueGetter::readIntegerReply(RedisModuleCallReply* reply) -> long long {
	if (reply == nullptr || RedisModule_CallReplyType(reply) != REDISMODULE_REPLY_INTEGER) {
		return -1;
	}

	return RedisModule_CallReplyInteger(reply);
}

auto ValueGetter::getKeyType(RedisModuleString* keyName) const -> int {
	const auto handle = RedisModule_OpenKey(ctx, keyName, REDISMODULE_READ | REDISMODULE_OPEN_KEY_NOEFFECTS);

	const auto keyType = RedisModule_KeyType(handle);

	RedisModule_CloseKey(handle);

	return keyType;
}
