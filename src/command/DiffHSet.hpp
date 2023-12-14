/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <vendor/redismodule.h>

inline constexpr auto DHSET_COMMAND_NAME = "DHSET";

extern "C" int DiffHSet(RedisModuleCtx* ctx, RedisModuleString** argv, int argc);
