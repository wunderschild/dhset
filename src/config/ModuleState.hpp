/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <optional>
#include <regex>
#include <unordered_set>

#include "ShrinkStrategy.hpp"
#include "KeyPattern.hpp"
#include "NotificationMode.hpp"
#include "Serializer.hpp"

typedef struct ModuleConfig {
	Serializer serializer = Serializer::JSON;

	NotificationMode notificationMode = NotificationMode::PER_CHANNEL;

	KeyPattern keyPattern = KeyPattern("");

	bool enableKeyCaching = false;
	ShrinkStrategy shrinkMatchedKeyCache = ShrinkStrategy::FLUSH;
	long long matchedKeyCacheLimit = 1000;

	std::optional<std::string> commandFilterTarget = std::nullopt;
} ModuleConfig;

const static inline auto DEFAULT_CONFIG = ModuleConfig{};

class ModuleStateHolder {
public:
	static ModuleConfig config;
	static std::unordered_set<std::string> acceptedKeys;
};

auto ensureKeyCacheSizeInLimits() -> void;

