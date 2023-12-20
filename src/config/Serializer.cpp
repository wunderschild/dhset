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

#include "ModuleState.hpp"
#include "util/util.hpp"

auto getSerializer(const char* /*unused*/, void* /*unused*/) {
	return static_cast<int>(ModuleStateHolder::config.serializer);
}

auto setSerializer(
	const char* /*unused*/,
	int value,
	void* /*unused*/,
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
		static_cast<const char **>(enumNames),
		static_cast<const int *>(enumValues),
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

