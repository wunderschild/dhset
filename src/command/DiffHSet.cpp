/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "DiffHSet.hpp"

#include <sstream>
#include <nlohmann/json.hpp>

#include "config.hpp"
#include "exec/ValueGetter.hpp"
#include "util/util.hpp"

using json = nlohmann::json;

auto serializePayload(RedisModuleCtx* ctx, const json& data, const Serializer serializer) {
	auto serializedPayload = std::string();

	switch (serializer) {
		case Serializer::CBOR:
			json::to_cbor(data, serializedPayload);
			break;
		case Serializer::MSGPack:
			json::to_msgpack(data, serializedPayload);
			break;
		case Serializer::JSON:
		default:
			serializedPayload = (std::ostringstream() << data).str();
	}

	return toRedisString(serializedPayload, ctx);
}

auto publishNotification(
	RedisModuleCtx* ctx,
	const RedisModuleString* keyName,
	const RedisHash& oldState,
	const RedisHash& newState,
	const ModuleConfig& config
) {
	if ((config.notificationMode & (NotificationMode::PER_KEY | NotificationMode::PER_CHANNEL)) <= 0) {
		return;
	}

	auto event = json();

	auto key = fromRedisString(keyName);
	event["k"] = key;
	event["o"] = oldState;
	event["u"] = newState;

	const auto payload = serializePayload(ctx, event, config.serializer);

	if ((config.notificationMode & NotificationMode::PER_KEY) > 0) {
		const auto channelName = (
			std::ostringstream()
			<< "__modevent@" << RedisModule_GetSelectedDb(ctx) << "__"
			<< ":dhset:" << key
		).str();

		publish(ctx, channelName, payload);
	}
	if ((config.notificationMode & NotificationMode::PER_CHANNEL) > 0) {
		const auto channelName = (
			std::ostringstream()
			<< "__modevent@" << RedisModule_GetSelectedDb(ctx) << "__"
			<< ":dhset"
		).str();

		publish(ctx, channelName, payload);
	}

	RedisModule_FreeString(ctx, payload);
}

auto computeNewState(
	const std::vector<std::string>& updates
) {
	auto updateHash = RedisHash();

	const auto size = updates.size();

	for (auto i = 0; i < size; i += 2) {
		updateHash[updates[i]] = updates[i + 1];
	}

	return updateHash;
}

auto DiffHSet(RedisModuleCtx* ctx, RedisModuleString** argv, const int argc) -> int {
	if (argc < 3 || argc % 2 != 0) {
		return RedisModule_WrongArity(ctx);
	}

	const auto getter = ValueGetter(ctx);

	const auto oldState = getter.readHashKey(argv[1]);

	const auto reply = RedisModule_Call(ctx, "HSET", "!Ev", argv + 1, argc - 1);

	if (
		ValueGetter::readIntegerReply(reply) >= 0
	) {
		auto updates = std::vector<std::string>();

		for (auto i = 2; i < argc; i++) {
			updates.emplace_back(fromRedisString(argv[i]));
		}

		const auto newState = computeNewState(updates);

		publishNotification(ctx, argv[1], oldState, newState, ConfigHolder::config);
	}

	RedisModule_ReplyWithCallReply(ctx, reply);

	return REDISMODULE_OK;
}
