/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "ModuleState.hpp"

#include "../util/util.hpp"

ModuleConfig ModuleStateHolder::config = ModuleConfig{};
std::unordered_set<std::string> ModuleStateHolder::acceptedKeys = {};
RedisModuleString* ModuleStateHolder::commandNameEncoded = nullptr;

auto ensureKeyCacheSizeInLimits() -> void {
	if (
		ModuleStateHolder::acceptedKeys.size() >=
		static_cast<std::size_t>(ModuleStateHolder::config.matchedKeyCacheLimit)
	) {
		applyStrategy(ModuleStateHolder::acceptedKeys, ModuleStateHolder::config.shrinkMatchedKeyCache);
	}
}
