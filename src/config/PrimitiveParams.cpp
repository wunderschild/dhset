/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "PrimitiveParams.hpp"

#include "util/util.hpp"

constexpr auto KEY_CACHE_SIZE_MIN = 8;
constexpr auto KEY_CACHE_SIZE_MAX = 65536;

auto getKeyCacheSize(const char*, void*) {
	return ModuleStateHolder::config.matchedKeyCacheLimit;
}

auto setKeyCacheSize(const char*, long long value, void*, RedisModuleString** err) {
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

auto getEnableKeyCaching(const char*, void*) {
	return static_cast<int>(ModuleStateHolder::config.enableKeyCaching);
}

auto setEnableKeyCaching(const char*, int value, void*, RedisModuleString**) {
	ModuleStateHolder::config.enableKeyCaching = value > 0;

	return REDISMODULE_OK;
}

auto registerEnableKeyCachingConfigOption(RedisModuleCtx* ctx) -> bool {
	if (auto res = RedisModule_RegisterBoolConfig(
		ctx,
		ENABLE_KEY_CACHING_OPTION,
		DEFAULT_CONFIG.enableKeyCaching,
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
