/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <string>
#include <regex>
#include <vendor/redismodule.h>

constexpr auto KEY_PATTERN_OPTION = "key-pattern";

class KeyPattern : public std::regex {
	std::string source;

public:
	explicit KeyPattern(const std::string& source);

	[[nodiscard]] auto getSource() const -> std::string;
};

auto registerKeyPatternConfigOption(RedisModuleCtx* ctx) -> bool;
