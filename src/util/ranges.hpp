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
auto joinToString(Range r, std::string_view delimiter) {
	std::ostringstream receiver;

	for (auto iter = std::begin(r); iter != std::end(r); ++iter) {
		if (iter == std::begin(r)) {
			receiver << *iter;
		} else {
			receiver << delimiter << *iter;
		}
	}

	return receiver.str();
}

#endif //RANGES_HPP
