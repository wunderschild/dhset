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

constexpr auto SERIALIZE_WITH_OPTION = "serialize-with";

enum class Serializer : int {
	JSON = 0,
	CBOR = 1,
	MSGPack = 2,
};

auto registerSerializerConfigOption(RedisModuleCtx* ctx) -> bool;
