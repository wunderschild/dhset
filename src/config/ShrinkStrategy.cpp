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
