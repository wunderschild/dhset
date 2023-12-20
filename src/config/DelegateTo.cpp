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

#include "DelegateTo.hpp"
#include "util/util.hpp"

#include <algorithm>
// ReSharper disable once CppUnusedIncludeDirective
#include <ranges>
#include <string_view>

#include "util/ranges.hpp"
#include "util/stringstream.hpp"

auto getDelegateTo(const char* /*unused*/, void* /*unused*/) {
	return toRedisString(ModuleStateHolder::config.delegateTo);
}

auto setDelegateTo(const char* /*unused*/, RedisModuleString* value, void* /*unused*/, RedisModuleString** err) {
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
