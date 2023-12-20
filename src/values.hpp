
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

#pragma once
#include <vendor/redismodule.h>

inline constexpr auto MODULE_NAME = "dhset";
inline constexpr auto MODULE_VER = 1;

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
} // namespace detail

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
