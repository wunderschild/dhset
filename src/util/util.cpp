/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "util.hpp"

#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>

auto bytesFromRedisString(const RedisModuleString* str) -> ByteArray {
	if (str == nullptr) {
		return {};
	}

	std::size_t len = 0;
	const auto* const buf = RedisModule_StringPtrLen(str, &len);

	auto receiver = ByteArray();

	for (std::size_t i = 0; i < len; ++i) {
		receiver.emplace_back(buf[i]);
	}

	return receiver;
}

auto fromRedisString(const RedisModuleString* str) -> std::string {
	if (str == nullptr) {
		return {};
	}

	std::size_t len = 0;
	const auto* const buf = RedisModule_StringPtrLen(str, &len);

	return {buf, len};
}

auto toRedisString(const std::string_view str) -> RedisModuleString* {
	return toRedisString(str, nullptr);
}

auto toRedisString(const std::ostringstream& str) -> RedisModuleString* {
	return toRedisString(str.str());
}

auto toRedisString(const std::string_view str, RedisModuleCtx* ctx) -> RedisModuleString* {
	return RedisModule_CreateString(ctx, str.data(), str.size());
}

auto debugSend(RedisModuleCtx* ctx, const std::string_view message) -> void {
	auto* const channel = toRedisString("dhset", ctx);
	auto* const rMessage = toRedisString(message, ctx);

	RedisModule_PublishMessage(ctx, channel, rMessage);

	RedisModule_FreeString(ctx, channel);
	RedisModule_FreeString(ctx, rMessage);
}

auto debugLogObject(const nlohmann::json& message) -> void {
	std::cerr << message << '\n';
}

auto publish(
	RedisModuleCtx* ctx,
	const std::string_view channel,
	const std::string_view data
) -> void {
	auto* const rData = toRedisString(data, ctx);

	publish(ctx, channel, rData);

	RedisModule_FreeString(ctx, rData);
}

auto publish(RedisModuleCtx* ctx, const std::string_view channel, RedisModuleString* data) -> void {
	auto* const rChannel = toRedisString(channel, ctx);

	RedisModule_PublishMessage(ctx, rChannel, data);

	RedisModule_FreeString(ctx, rChannel);
}

auto toLower(std::string_view str) -> std::string {
	std::string receiver;

	std::ranges::transform(str, std::back_inserter(receiver), [](auto chr) {
		return std::tolower(chr);
	});

	return receiver;
}

auto chrIEquals(const char chr1, const char chr2) {
	return std::tolower(chr1) == std::tolower(chr2);
}

auto strIEquals(const std::string_view str1, const std::string_view str2) -> bool {
	return std::equal(
		std::begin(str1), std::end(str1),
		std::begin(str2),
		chrIEquals
	);
}
