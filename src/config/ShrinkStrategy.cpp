/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "ShrinkStrategy.hpp"
#include "ModuleState.hpp"

#include "util/util.hpp"

auto getKeyCacheFlushStrategy(const char* /*unused*/, void* /*unused*/) {
	return static_cast<int>(ModuleStateHolder::config.shrinkMatchedKeyCache);
}

auto setKeyCacheFlushStrategy(const char* /*unused*/, int value, void* /*unused*/, RedisModuleString** err) {
	switch (const auto strat = static_cast<ShrinkStrategy>(value)) {
		case ShrinkStrategy::FLUSH:
		case ShrinkStrategy::HALVE:
		case ShrinkStrategy::ONE_OUT:
			ModuleStateHolder::config.shrinkMatchedKeyCache = strat;
			ensureKeyCacheSizeInLimits();
			return REDISMODULE_OK;
	}

	*err = toRedisString("Invalid value passed!");

	return REDISMODULE_ERR;
}

auto registerKeyCacheShrinkConfigOption(RedisModuleCtx* ctx) -> bool {
	const char* enumNames[] = {"flush", "halve", "one-out"};
	constexpr int enumValues[] = {
		static_cast<int>(ShrinkStrategy::FLUSH),
		static_cast<int>(ShrinkStrategy::HALVE),
		static_cast<int>(ShrinkStrategy::ONE_OUT),
	};

	if (const auto res = RedisModule_RegisterEnumConfig(
		ctx,
		SHRINK_KEY_CACHE_OPTION,
		static_cast<int>(DEFAULT_CONFIG.shrinkMatchedKeyCache),
		REDISMODULE_CONFIG_DEFAULT,
		static_cast<const char **>(enumNames),
		static_cast<const int *>(enumValues),
		3,
		getKeyCacheFlushStrategy,
		setKeyCacheFlushStrategy,
		nullptr,
		nullptr
	); res != REDISMODULE_OK) {
		RedisModule_Log(ctx, REDISMODULE_LOGLEVEL_WARNING, "Failed to register config parameter");

		return false;
	}

	return true;
}
