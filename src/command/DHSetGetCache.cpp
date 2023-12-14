/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "DHSetGetCache.hpp"

#include "config/ModuleState.hpp"

auto commandDiffHSetGetCache(RedisModuleCtx* ctx, RedisModuleString** /*unused*/, const int /*unused*/) -> int {
    const auto size = ModuleStateHolder::acceptedKeys.size();

    RedisModule_ReplyWithMap(ctx, static_cast<long>(size));

    for (auto [key, matched]: ModuleStateHolder::acceptedKeys) {
        RedisModule_ReplyWithSimpleString(ctx, key.data());
        RedisModule_ReplyWithBool(ctx, static_cast<int>(matched));
    }

    return REDISMODULE_OK;
}
