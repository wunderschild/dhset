/*
* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

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
