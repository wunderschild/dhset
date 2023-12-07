/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once
#include "ModuleState.hpp"

constexpr auto MATCHED_KEY_CACHE_SIZE_OPTION = "matched-key-cache-size";
constexpr auto ENABLE_KEY_CACHING_OPTION = "enable-key-caching";

auto registerKeyCacheSizeConfigOption(RedisModuleCtx* ctx) -> bool;

auto registerEnableKeyCachingConfigOption(RedisModuleCtx* ctx) -> bool;
