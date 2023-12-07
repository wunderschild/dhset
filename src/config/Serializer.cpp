/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ModuleState.hpp"
#include "util/util.hpp"

auto getSerializer(const char*, void*) {
	return static_cast<int>(ModuleStateHolder::config.serializer);
}

auto setSerializer(
	const char*,
	int value,
	void*,
	RedisModuleString** err
) {
	switch (const auto serializer = static_cast<Serializer>(value)) {
		case Serializer::JSON:
		case Serializer::CBOR:
		case Serializer::MSGPack:
			ModuleStateHolder::config.serializer = serializer;
			return REDISMODULE_OK;
	}

	*err = toRedisString(
		std::string("Invalid config value for ") + SERIALIZE_WITH_OPTION
	);

	return REDISMODULE_ERR;
}

auto registerSerializerConfigOption(RedisModuleCtx* ctx) -> bool {
	const char* enumNames[] = {"json", "cbor", "msgpack"};
	constexpr int enumValues[] = {
		static_cast<int>(Serializer::JSON),
		static_cast<int>(Serializer::CBOR),
		static_cast<int>(Serializer::MSGPack)
	};

	if (const auto res = RedisModule_RegisterEnumConfig(
		ctx,
		SERIALIZE_WITH_OPTION,
		static_cast<int>(DEFAULT_CONFIG.serializer),
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

