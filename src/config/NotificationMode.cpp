/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ModuleState.hpp"

auto getNotificationMode(const char*, void*) {
	return static_cast<int>(ModuleStateHolder::config.notificationMode);
}

auto setNotificationMode(const char*, int value, void*, RedisModuleString**) {
	ModuleStateHolder::config.notificationMode =
		value & (NotificationMode::PER_KEY | NotificationMode::PER_CHANNEL);

	return REDISMODULE_OK;
}

auto registerNotificationModeConfigOption(RedisModuleCtx* ctx) -> bool {
	const char* enumNames[] = {"key", "channel"};
	constexpr int enumValues[] = {
		static_cast<int>(NotificationMode::PER_KEY),
		static_cast<int>(NotificationMode::PER_CHANNEL),
	};

	if (const auto res = RedisModule_RegisterEnumConfig(
		ctx,
		NOTIFICATION_MODE_OPTION,
		static_cast<int>(DEFAULT_CONFIG.notificationMode),
		REDISMODULE_CONFIG_BITFLAGS,
		enumNames,
		enumValues,
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
