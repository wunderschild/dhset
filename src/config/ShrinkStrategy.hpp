/*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vendor/redismodule.h>

constexpr auto SHRINK_KEY_CACHE_OPTION = "shrink-matched-key-cache";

enum class ShrinkStrategy : int {
	FLUSH = 0,
	HALVE = 1,
	ONE_OUT = 2,
};

auto registerKeyCacheShrinkConfigOption(RedisModuleCtx* ctx) -> bool;

template<typename K, typename V>
auto applyStrategy(
	std::unordered_map<K, V>& map,
	const ShrinkStrategy strategy
) {
	switch (strategy) {
		case ShrinkStrategy::FLUSH:
			map.clear();
			break;
		case ShrinkStrategy::HALVE: {
			auto size = map.size() >> 1;
			for (size_t i = 0; i < size; i++) {
				map.erase(std::begin(map));
			}
			break;
		}
		case ShrinkStrategy::ONE_OUT:
			map.erase(std::begin(map));
	}
}
