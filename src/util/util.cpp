/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "util.hpp"

#include <iostream>
#include <nlohmann/json.hpp>

auto fromRedisString(const RedisModuleString* str) -> std::string {
	if (str == nullptr) {
		return {};
	}

	size_t len;
	const auto buf = RedisModule_StringPtrLen(str, &len);

	return {buf, len};
}

auto toRedisString(const std::string_view str) -> RedisModuleString* {
	return toRedisString(str, nullptr);
}

auto toRedisString(const std::string_view str, RedisModuleCtx* ctx) -> RedisModuleString* {
	return RedisModule_CreateString(ctx, str.data(), str.size());
}

auto debugSend(RedisModuleCtx* ctx, const std::string_view message) -> void {
	const auto channel = toRedisString("dhset", ctx);
	const auto rMessage = toRedisString(message, ctx);

	RedisModule_PublishMessage(ctx, channel, rMessage);

	RedisModule_FreeString(ctx, channel);
	RedisModule_FreeString(ctx, rMessage);
}

auto debugLogObject(const nlohmann::json& message) -> void {
	std::cerr << message << std::endl;
}

auto publish(
	RedisModuleCtx* ctx,
	std::string_view channel,
	std::string_view data
) -> void {
	const auto rData = toRedisString(channel, ctx);

	publish(ctx, channel, rData);

	RedisModule_FreeString(ctx, rData);
}

auto publish(RedisModuleCtx* ctx, std::string_view channel, RedisModuleString* data) -> void {
	const auto rChannel = toRedisString(channel, ctx);

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

auto chrIEquals(const char a, const char b) {
	return std::tolower(a) == std::tolower(b);
}

auto strIEquals(const std::string_view a, const std::string_view b) -> bool {
	return std::equal(
		std::begin(a), std::end(a),
		std::begin(b),
		chrIEquals
	);
}
