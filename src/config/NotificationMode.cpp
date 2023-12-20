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

#include "ModuleState.hpp"

auto getNotificationMode(const char* /*unused*/, void* /*unused*/) {
	return static_cast<int>(ModuleStateHolder::config.notificationMode);
}

auto setNotificationMode(const char* /*unused*/, int value, void* /*unused*/, RedisModuleString** /*unused*/) {
	ModuleStateHolder::config.notificationMode =
		value & (NotificationMode::PER_KEY | NotificationMode::PER_CHANNEL);

	return REDISMODULE_OK;
}

auto registerNotificationModeConfigOption(RedisModuleCtx* ctx) -> bool {
	const char* enumNames[] = {"key", "channel"};
	const int enumValues[] = {
		static_cast<int>(NotificationMode::PER_KEY),
		static_cast<int>(NotificationMode::PER_CHANNEL),
	};

	if (const auto res = RedisModule_RegisterEnumConfig(
		ctx,
		NOTIFICATION_MODE_OPTION,
		static_cast<int>(DEFAULT_CONFIG.notificationMode),
		REDISMODULE_CONFIG_BITFLAGS,
		static_cast<const char **>(enumNames),
		static_cast<const int *>(enumValues),
		2,
		getNotificationMode,
		setNotificationMode,
		nullptr,
		ctx
	); res != REDISMODULE_OK) {
		RedisModule_Log(ctx, REDISMODULE_LOGLEVEL_WARNING, "Failed to register config variable");

		return false;
	}

	return true;
}
