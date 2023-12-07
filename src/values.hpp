
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once
#include <vendor/redismodule.h>

inline constexpr auto MODULE_NAME = "dhset";
inline constexpr auto MODULE_VER = 1;

inline constexpr auto COMMAND_NAME = "DHSET";

namespace detail {
	inline constexpr RedisModuleCommandArg* BLOCK_SUBARGS = (RedisModuleCommandArg[]){
		RedisModuleCommandArg{
			.name = "field",
			.type = REDISMODULE_ARG_TYPE_STRING,
		},
		RedisModuleCommandArg{
			.name = "value",
			.type = REDISMODULE_ARG_TYPE_STRING,
		},
		nullptr
	};

	inline constexpr RedisModuleCommandArg* COMMAND_ARGS = (RedisModuleCommandArg[]){
		RedisModuleCommandArg{
			.name = "key",
			.type = REDISMODULE_ARG_TYPE_KEY,
		},
		RedisModuleCommandArg{
			.name = "field",
			.type = REDISMODULE_ARG_TYPE_BLOCK,
			.flags = REDISMODULE_CMD_ARG_MULTIPLE,
			.subargs = BLOCK_SUBARGS,
		},
		nullptr
	};
}

inline constexpr auto COMMAND_INFO = RedisModuleCommandInfo{
	.version = REDISMODULE_COMMAND_INFO_VERSION,
	.summary = R"(Observable HSET command.
Emits a notification on every successful invocation, containing old and new value for the key.
A notification is sent to a channel `__modevent@{db}__:dhset`, with payload in JSON format.
)",
	.complexity = "Effectively same as HSET O(N), albeit payload serialization may take some time.",
	.since = "v1",
	.arity = -3,
	.args = detail::COMMAND_ARGS,
};
