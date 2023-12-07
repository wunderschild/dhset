/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

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
