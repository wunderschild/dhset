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
