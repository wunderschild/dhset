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
#include "PrimitiveParams.hpp"

#include "util/util.hpp"

constexpr auto KEY_CACHE_SIZE_MIN = 8;
constexpr auto KEY_CACHE_SIZE_MAX = 65536;

auto getKeyCacheSize(const char* /*unused*/, void* /*unused*/) {
	return ModuleStateHolder::config.matchedKeyCacheLimit;
}

auto setKeyCacheSize(const char* /*unused*/, long long value, void* /*unused*/, RedisModuleString** err) {
	if (value < KEY_CACHE_SIZE_MIN || value > KEY_CACHE_SIZE_MAX) {
		*err = RedisModule_CreateStringPrintf(
			nullptr,
			"Please, provide a value between %d and %d",
			KEY_CACHE_SIZE_MIN, KEY_CACHE_SIZE_MAX
		);

		return REDISMODULE_ERR;
	}

	ModuleStateHolder::config.matchedKeyCacheLimit = value;

	ensureKeyCacheSizeInLimits();

	return REDISMODULE_OK;
}

auto registerKeyCacheSizeConfigOption(RedisModuleCtx* ctx) -> bool {
	if (const auto res = RedisModule_RegisterNumericConfig(
		ctx,
		MATCHED_KEY_CACHE_SIZE_OPTION,
		DEFAULT_CONFIG.matchedKeyCacheLimit,
		REDISMODULE_CONFIG_DEFAULT,
		KEY_CACHE_SIZE_MIN, KEY_CACHE_SIZE_MAX,
		getKeyCacheSize,
		setKeyCacheSize,
		nullptr,
		nullptr
	); res != REDISMODULE_OK) {
		RedisModule_Log(ctx, REDISMODULE_LOGLEVEL_WARNING, "Failed to register config parameter");
		return false;
	}

	return true;
}

auto getEnableKeyCaching(const char* /*unused*/, void* /*unused*/) {
	return static_cast<int>(ModuleStateHolder::config.enableKeyCaching);
}

auto setEnableKeyCaching(const char* /*unused*/, int value, void* /*unused*/, RedisModuleString** /*unused*/) {
	ModuleStateHolder::config.enableKeyCaching = value > 0;

	return REDISMODULE_OK;
}

auto registerEnableKeyCachingConfigOption(RedisModuleCtx* ctx) -> bool {
	if (auto res = RedisModule_RegisterBoolConfig(
		ctx,
		ENABLE_KEY_CACHING_OPTION,
		static_cast<int>(DEFAULT_CONFIG.enableKeyCaching),
		REDISMODULE_CONFIG_DEFAULT,
		getEnableKeyCaching,
		setEnableKeyCaching,
		nullptr,
		nullptr
	); res != REDISMODULE_OK) {
		RedisModule_Log(ctx, REDISMODULE_LOGLEVEL_WARNING, "Failed to register config option!");
		return false;
	}

	return true;
}
