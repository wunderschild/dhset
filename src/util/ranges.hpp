/*
	This Source Code Form is subject to the terms of the Mozilla Public
	License, v. 2.0. If a copy of the MPL was not distributed with this
	file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

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
