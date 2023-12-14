/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "DiffHSet.hpp"

#include <sstream>
#include <nlohmann/json.hpp>

#include "config/ModuleState.hpp"
#include "exec/ValueGetter.hpp"
#include "util/util.hpp"

using Json = nlohmann::json;

auto serializePayload(RedisModuleCtx* ctx, const Json& data, const Serializer serializer) {
	auto serializedPayload = std::string();

	switch (serializer) {
		case Serializer::CBOR:
			Json::to_cbor(data, serializedPayload);
			break;
		case Serializer::MSGPack:
			Json::to_msgpack(data, serializedPayload);
			break;
		case Serializer::JSON:
		default:
			serializedPayload = (std::ostringstream() << data).str();
	}

	return toRedisString(serializedPayload, ctx);
}

auto publishNotification(
	RedisModuleCtx* ctx,
	std::string_view key,
	const RedisHash& oldState,
	const RedisHash& newState,
	const ModuleConfig& config
) {
	if ((config.notificationMode & (NotificationMode::PER_KEY | NotificationMode::PER_CHANNEL)) <= 0) {
		return;
	}

	auto event = Json();

	event["k"] = key;
	event["o"] = oldState;
	event["u"] = newState;

	auto* const payload = serializePayload(ctx, event, config.serializer);

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

auto shouldReportKey(const std::string& key) {
	if (ModuleStateHolder::config.keyPattern.getSource().empty()) {
		return true;
	}

	if (!ModuleStateHolder::config.enableKeyCaching) {
		return std::regex_match(
			std::begin(key), std::end(key),
			ModuleStateHolder::config.keyPattern
		);
	}

	if (ModuleStateHolder::acceptedKeys.contains(key)) {
		return ModuleStateHolder::acceptedKeys[key];
	}

	const auto matches = std::regex_match(
		std::begin(key), std::end(key),
		ModuleStateHolder::config.keyPattern
	);

	ensureKeyCacheSizeInLimits();

	return ModuleStateHolder::acceptedKeys[key] = matches;
}

auto invokeActual(
	const std::string_view command,
	RedisModuleCtx* ctx,
	RedisModuleString** argv,
	const int argc
) -> RedisModuleCallReply* {
	return RedisModule_Call(ctx, command.data(), "!Ev", argv + 1, argc - 1);
}

auto commandDiffHSet(RedisModuleCtx* ctx, RedisModuleString** argv, const int argc) -> int {
	if (argc < 3 || argc % 2 != 0) {
		return RedisModule_WrongArity(ctx);
	}

	const auto key = fromRedisString(argv[1]);

	const auto& cmdActual = ModuleStateHolder::config.delegateTo;

	if (!shouldReportKey(key)) {
		RedisModule_ReplyWithCallReply(ctx, invokeActual(cmdActual, ctx, argv, argc));

		return REDISMODULE_OK;
	}

	const auto getter = ValueGetter(ctx);

	const auto oldState = getter.readHashKey(argv[1]);

	auto* const reply = invokeActual(cmdActual, ctx, argv, argc);

	if (
		const auto replyType = RedisModule_CallReplyType(reply);
		replyType != REDISMODULE_REPLY_UNKNOWN
		&& replyType != REDISMODULE_REPLY_ERROR
	) {
		auto newState = RedisHash();

		for (auto i = 2; i < argc; i += 2) {
			newState.insert({
				fromRedisString(argv[i]),
				bytesFromRedisString(argv[i + 1])
			});
		}

		publishNotification(ctx, key, oldState, newState, ModuleStateHolder::config);
	}

	RedisModule_ReplyWithCallReply(ctx, reply);

	return REDISMODULE_OK;
}
