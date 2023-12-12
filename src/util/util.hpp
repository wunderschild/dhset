/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <string>
#include <nlohmann/json_fwd.hpp>

#include <vendor/redismodule.h>

auto fromRedisString(const RedisModuleString* str) -> std::string;

auto toRedisString(std::string_view str) -> RedisModuleString*;

auto toRedisString(std::string_view str, RedisModuleCtx* ctx) -> RedisModuleString*;

auto debugSend(RedisModuleCtx* ctx, std::string_view message) -> void;

auto debugLogObject(const nlohmann::json& message) -> void;

auto publish(RedisModuleCtx* ctx, std::string_view channel, std::string_view data) -> void;

auto publish(RedisModuleCtx* ctx, std::string_view channel, RedisModuleString* data) -> void;

auto toLower(std::string_view) -> std::string;

auto strIEquals(std::string_view a, std::string_view b) -> bool;