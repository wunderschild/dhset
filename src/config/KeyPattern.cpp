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

#include "KeyPattern.hpp"

#include "ModuleState.hpp"
#include "util/util.hpp"

KeyPattern::KeyPattern(const std::string& source) : std::regex(source.data()), source(source) {
}

auto KeyPattern::getSource() const -> std::string {
	return source;
}

auto getKeyPattern(const char* /*unused*/, void* /*unused*/) {
	return toRedisString(ModuleStateHolder::config.keyPattern.getSource());
}

auto setKeyPattern(const char* /*unused*/, RedisModuleString* value, void* /*unused*/,
                   RedisModuleString** err) {
	try {
		ModuleStateHolder::config.keyPattern = KeyPattern(fromRedisString(value));
		ModuleStateHolder::acceptedKeys.clear();
	} catch (const std::regex_error& er) {
		*err = toRedisString(er.what());

		return REDISMODULE_ERR;
	}

	return REDISMODULE_OK;
}

auto registerKeyPatternConfigOption(RedisModuleCtx* ctx) -> bool {
	if (const auto res = RedisModule_RegisterStringConfig(
		ctx,
		KEY_PATTERN_OPTION,
		DEFAULT_CONFIG.keyPattern.getSource().data(),
		REDISMODULE_CONFIG_DEFAULT,
		getKeyPattern,
		setKeyPattern,
		nullptr,
		nullptr
	); res != REDISMODULE_OK) {
		RedisModule_Log(ctx, REDISMODULE_LOGLEVEL_WARNING, "Failed to register %s option!", KEY_PATTERN_OPTION);
		return false;
	}

	return true;
}
