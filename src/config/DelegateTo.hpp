/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once
#include "ModuleState.hpp"

constexpr auto ALLOWED_DELEGATES = {"hset", "hmset"};

constexpr auto DELEGATE_TO_OPTION = "delegate-to";

auto registerDelegateToConfigOption(RedisModuleCtx* ctx) -> bool;
