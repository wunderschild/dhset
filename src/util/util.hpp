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

#include <string>
#include <nlohmann/json.hpp>

#include <vendor/redismodule.h>

using ByteArray = nlohmann::json::binary_t;

auto bytesFromRedisString(const RedisModuleString* str) -> ByteArray;

auto fromRedisString(const RedisModuleString* str) -> std::string;

auto toRedisString(std::string_view str) -> RedisModuleString*;

auto toRedisString(const std::ostringstream& str) -> RedisModuleString*;

auto toRedisString(std::string_view str, RedisModuleCtx* ctx) -> RedisModuleString*;

auto debugSend(RedisModuleCtx* ctx, std::string_view message) -> void;

auto debugLogObject(const nlohmann::json& message) -> void;

auto publish(RedisModuleCtx* ctx, std::string_view channel, std::string_view data) -> void;

auto publish(RedisModuleCtx* ctx, std::string_view channel, RedisModuleString* data) -> void;

auto toLower(std::string_view) -> std::string;

auto strIEquals(std::string_view str1, std::string_view str2) -> bool;
