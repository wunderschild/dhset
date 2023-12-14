/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "ModuleState.hpp"

#include "../util/util.hpp"

ModuleConfig ModuleStateHolder::config = ModuleConfig{}; // NOLINT(*-avoid-non-const-global-variables, *-err58-cpp)
std::unordered_map<std::string, bool> ModuleStateHolder::acceptedKeys = // NOLINT(*-avoid-non-const-global-variables)
	{};

auto ensureKeyCacheSizeInLimits() -> void {
	if (
		ModuleStateHolder::acceptedKeys.size() >=
		static_cast<std::size_t>(ModuleStateHolder::config.matchedKeyCacheLimit)
	) {
		applyStrategy(ModuleStateHolder::acceptedKeys, ModuleStateHolder::config.shrinkMatchedKeyCache);
	}
}
