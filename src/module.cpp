/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#define REDISMODULE_API extern "C"

#include <vendor/redismodule.h>
#include "values.hpp"

#include "command/DiffHSet.hpp"
#include "command/CommandFilter.hpp"
#include "command/DHSetGetCache.hpp"
#include "config/DelegateTo.hpp"
#include "config/PrimitiveParams.hpp"
#include "util/util.hpp"

extern "C" [[maybe_unused]] int RedisModule_OnLoad( // NOLINT(*-use-trailing-return-type, *-identifier-naming)
	RedisModuleCtx* ctx,
	RedisModuleString** argv,
	int argc
) {
	if (RedisModule_Init(ctx, MODULE_NAME, MODULE_VER, REDISMODULE_APIVER_1) == REDISMODULE_ERR) {
		return REDISMODULE_ERR;
	}

	if (
		RedisModule_CreateCommand(
			ctx,
			DHSET_COMMAND_NAME,
			commandDiffHSet,
			"write deny-oom",
			1, 1, 1
		) != REDISMODULE_OK
	) {
		RedisModule_Log(
			ctx, REDISMODULE_LOGLEVEL_WARNING,
			"Failed to register command!"
		);
		return REDISMODULE_ERR;
	}

	if (
		RedisModule_CreateCommand(
			ctx,
			DHSET_GET_CACHE_COMMAND_NAME,
			commandDiffHSetGetCache,
			"deny-oom",
			0, 0, 0
		) != REDISMODULE_OK
	) {
		RedisModule_Log(
			ctx, REDISMODULE_LOGLEVEL_WARNING,
			"Failed to register debug command!"
		);
	}

	if (auto* const command = RedisModule_GetCommand(ctx, DHSET_COMMAND_NAME); command != nullptr) {
		RedisModule_SetCommandInfo(command, &COMMAND_INFO);
	}

	if (
		!registerSerializerConfigOption(ctx)
		|| !registerNotificationModeConfigOption(ctx)
		|| !registerKeyPatternConfigOption(ctx)
		|| !registerKeyCacheShrinkConfigOption(ctx)
		|| !registerKeyCacheSizeConfigOption(ctx)
		|| !registerEnableKeyCachingConfigOption(ctx)
		|| !registerDelegateToConfigOption(ctx)
	) {
		RedisModule_Log(ctx, REDISMODULE_LOGLEVEL_WARNING, "Failed to register config options!");
		return REDISMODULE_ERR;
	}

	if (argc > 0) {
		ModuleStateHolder::config.commandFilterTarget =
			toLower(fromRedisString(argv[0])); // NOLINT(*-pro-bounds-pointer-arithmetic)

		if (auto *const filter = RedisModule_RegisterCommandFilter(
			ctx,
			commandFilter,
			REDISMODULE_CMDFILTER_NOSELF
		); filter == nullptr) {
			RedisModule_Log(ctx, REDISMODULE_LOGLEVEL_WARNING, "Failed to register command filter!");
		}
	}

	RedisModule_LoadConfigs(ctx);

	return REDISMODULE_OK;
}
