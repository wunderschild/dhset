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

#ifndef RANGES_HPP
#define RANGES_HPP

#include <string_view>
#include <sstream>

template<typename Range>
auto joinToString(Range range, std::string_view delimiter) {
	std::ostringstream receiver;

	for (auto iter = std::begin(range); iter != std::end(range); ++iter) {
		if (iter == std::begin(range)) {
			receiver << *iter;
		} else {
			receiver << delimiter << *iter;
		}
	}

	return receiver.str();
}

#endif //RANGES_HPP
