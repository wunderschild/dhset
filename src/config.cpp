/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "config.hpp"

#include "util/util.hpp"

ModuleConfig ConfigHolder::config = ModuleConfig{};

auto getSerializer(const char*, void*) {
	return static_cast<int>(ConfigHolder::config.serializer);
}

auto setSerializer(
	const char*,
	int value,
	void* privData,
	RedisModuleString** err
) {
	const auto ctx = static_cast<RedisModuleCtx *>(privData);

	switch (const auto serializer = static_cast<Serializer>(value)) {
		case Serializer::JSON:
		case Serializer::CBOR:
		case Serializer::MSGPack:
			ConfigHolder::config.serializer = serializer;
			return REDISMODULE_OK;
	}

	*err = toRedisString(
		std::string("Invalid config value for ") + SERIALIZER_CONFIG, ctx
	);

	return REDISMODULE_ERR;
}

auto registerSerializerConfig(RedisModuleCtx* ctx) -> bool {
	const char* enumNames[] = {"json", "cbor", "msgpack"};
	constexpr int enumValues[] = {
		static_cast<int>(Serializer::JSON),
		static_cast<int>(Serializer::CBOR),
		static_cast<int>(Serializer::MSGPack)
	};

	if (const auto res = RedisModule_RegisterEnumConfig(
		ctx,
		SERIALIZER_CONFIG,
		static_cast<int>(DefaultConfig.serializer),
		REDISMODULE_CONFIG_DEFAULT,
		enumNames,
		enumValues,
		3,
		getSerializer,
		setSerializer,
		nullptr,
		ctx
	); res != REDISMODULE_OK) {
		RedisModule_Log(ctx, REDISMODULE_LOGLEVEL_WARNING, "Failed to register config variable");

		return false;
	}

	return true;
}

auto getNotificationMode(const char*, void* data) {
	return static_cast<int>(ConfigHolder::config.notificationMode);
}

auto setNotificationMode(const char*, int value, void* data, RedisModuleString**) {
	ConfigHolder::config.notificationMode =
		value & (NotificationMode::PER_KEY | NotificationMode::PER_CHANNEL);

	return REDISMODULE_OK;
}

auto registerNotificationModeConfig(RedisModuleCtx* ctx) -> bool {
	const char* enumNames[] = {"key", "channel"};
	constexpr int enumValues[] = {
		static_cast<int>(NotificationMode::PER_KEY),
		static_cast<int>(NotificationMode::PER_CHANNEL),
	};

	if (const auto res = RedisModule_RegisterEnumConfig(
		ctx,
		NOTIFICATION_MODE_CONFIG,
		static_cast<int>(DefaultConfig.notificationMode),
		REDISMODULE_CONFIG_BITFLAGS,
		enumNames,
		enumValues,
		2,
		getNotificationMode,
		setNotificationMode,
		nullptr,
		ctx
	); res != REDISMODULE_OK) {
		RedisModule_Log(ctx, REDISMODULE_LOGLEVEL_WARNING, "Failed to register config variable");

		return false;
	}

	return true;
}

