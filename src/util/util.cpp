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
