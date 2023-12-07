/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#define REDISMODULE_API extern "C"

#include <vendor/redismodule.h>
#include "values.hpp"

#include "command/DiffHSet.hpp"
#include "config/PrimitiveParams.hpp"

extern "C" [[maybe_unused]] int RedisModule_OnLoad(RedisModuleCtx* ctx, RedisModuleString**, int) {
	if (RedisModule_Init(ctx, MODULE_NAME, MODULE_VER, REDISMODULE_APIVER_1) == REDISMODULE_ERR) {
		return REDISMODULE_ERR;
	}

	if (
		RedisModule_CreateCommand(
			ctx,
			COMMAND_NAME,
			DiffHSet,
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

	if (const auto command = RedisModule_GetCommand(ctx, COMMAND_NAME); command != nullptr) {
		RedisModule_SetCommandInfo(command, &COMMAND_INFO);
	}

	if (
		!registerSerializerConfigOption(ctx)
		|| !registerNotificationModeConfigOption(ctx)
		|| !registerKeyPatternConfigOption(ctx)
		|| !registerKeyCacheShrinkConfigOption(ctx)
		|| !registerKeyCacheSizeConfigOption(ctx)
		|| !registerEnableKeyCachingConfigOption(ctx)
	) {
		RedisModule_Log(ctx, REDISMODULE_LOGLEVEL_WARNING, "Failed to register config options!");
		return REDISMODULE_ERR;
	}

	RedisModule_LoadConfigs(ctx);

	return REDISMODULE_OK;
}
