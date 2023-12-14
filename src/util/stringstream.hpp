/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

#include <sstream>

template<typename Type>
auto operator<<(std::ostringstream& stream, const Type& type) -> std::ostringstream& {
    static_cast<std::ostream &>(stream) << type;
    return stream;
}

template<typename Type>
auto operator<<(std::ostringstream&& stream, const Type& type) -> std::ostringstream& {
    static_cast<std::ostream &>(stream) << type;
    return stream;
}
