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

#include "CommandFilter.hpp"

#include "DiffHSet.hpp"
#include "config/ModuleState.hpp"
#include "util/util.hpp"

void commandFilter(RedisModuleCommandFilterCtx* fctx) {
	if (const auto filterTarget = ModuleStateHolder::config.commandFilterTarget;
		filterTarget.has_value()) {
		auto *const cmdNameRef = RedisModule_CommandFilterArgGet(fctx, 0);
		std::size_t cmdNameLen = 0;
		const auto *const cmdName = RedisModule_StringPtrLen(cmdNameRef, &cmdNameLen);

		const auto& filterTargetValue = filterTarget.value();

		if (cmdNameLen != filterTargetValue.length()) { return;
}

		if (
			strIEquals(cmdName, filterTargetValue)
		) {
			RedisModule_CommandFilterArgReplace(fctx, 0, toRedisString(DHSET_COMMAND_NAME));
		}
	}
}
