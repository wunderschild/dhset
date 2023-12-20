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
#include <vendor/redismodule.h>

constexpr auto NOTIFICATION_MODE_OPTION = "notification-mode";

enum class NotificationMode : int {
	PER_KEY = 1,
	PER_CHANNEL = 2,
	BOTH = 3
};

inline auto operator|(const int lh, const NotificationMode rh) {
	return static_cast<NotificationMode>(lh | static_cast<int>(rh));
}

inline auto operator|(const NotificationMode lh, const NotificationMode rh) {
	return static_cast<int>(lh) | rh;
}

inline auto operator&(const int lh, const NotificationMode rh) {
	return static_cast<NotificationMode>(lh & static_cast<int>(rh));
}

inline auto operator&(const NotificationMode lh, const NotificationMode rh) {
	return static_cast<int>(lh) & rh;
}

inline auto operator>(const NotificationMode lh, const int rh) {
	return static_cast<int>(lh) > rh;
}

inline auto operator<=(const NotificationMode lh, const int rh) {
	return static_cast<int>(lh) <= rh;
}

auto registerNotificationModeConfigOption(RedisModuleCtx* ctx) -> bool;
