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
