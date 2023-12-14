/*
	This Source Code Form is subject to the terms of the Mozilla Public
	License, v. 2.0. If a copy of the MPL was not distributed with this
	file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "DelegateTo.hpp"
#include "util/util.hpp"

#include <algorithm>
#include <ranges>
#include <string_view>

#include "util/ranges.hpp"

auto getDelegateTo(const char*, void*) {
	return toRedisString(ModuleStateHolder::config.delegateTo);
}

auto setDelegateTo(const char*, RedisModuleString* value, void*, RedisModuleString** err) {
	const auto valueIn = toLower(fromRedisString(value));

	if (
		std::ranges::any_of(
			ALLOWED_DELEGATES,
			[valueIn](auto allowed) {
				return valueIn == allowed;
			}
		)
	) {
		ModuleStateHolder::config.delegateTo = valueIn;

		return REDISMODULE_OK;
	}

	*err = toRedisString(
		std::ostringstream()
		<< "Invalid value '" << valueIn << "'. "
		<< "Supported values are: " << joinToString(ALLOWED_DELEGATES, ", ") << "."
	);

	return REDISMODULE_ERR;
}

auto registerDelegateToConfigOption(RedisModuleCtx* ctx) -> bool {
	if (const auto res = RedisModule_RegisterStringConfig(
		ctx,
		DELEGATE_TO_OPTION,
		DEFAULT_CONFIG.delegateTo.data(),
		REDISMODULE_CONFIG_DEFAULT,
		getDelegateTo,
		setDelegateTo,
		nullptr,
		ctx
	); res != REDISMODULE_OK) {
		RedisModule_Log(ctx, REDISMODULE_LOGLEVEL_WARNING, "Failed to register %s option!", DELEGATE_TO_OPTION);
		return false;
	}

	return true;
}