/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#define REDISMODULE_API extern "C"

#include <cerrno>
#include <csignal>
#include <cstring>
#include <system_error>

#include "config.hpp"
#include "values.hpp"
#include "vendor/redismodule.h"

#include "command/DiffHSet.hpp"
#include "util/util.hpp"

constexpr auto commandName = "DHSET";

extern "C" int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
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
		!registerSerializerConfig(ctx) || !registerNotificationModeConfig(ctx)
		) {
		return REDISMODULE_ERR;
	}

	RedisModule_LoadConfigs(ctx);

	return REDISMODULE_OK;
}
