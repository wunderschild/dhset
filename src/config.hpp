/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "config/NotificationMode.hpp"
#include "config/Serializer.hpp"
#include "vendor/redismodule.h"

constexpr auto SERIALIZER_CONFIG = "serialize-with";
constexpr auto NOTIFICATION_MODE_CONFIG = "notification-mode";

typedef struct ModuleConfig {
	Serializer serializer = Serializer::JSON;
	NotificationMode notificationMode = NotificationMode::PER_CHANNEL;
} ModuleConfig;

constexpr auto DefaultConfig = ModuleConfig{};

class ConfigHolder {
public:
	static ModuleConfig config;
};

auto registerSerializerConfig(RedisModuleCtx* ctx) -> bool;

auto registerNotificationModeConfig(RedisModuleCtx* ctx) -> bool;
