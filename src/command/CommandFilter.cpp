//
// Created by kreme on 12.12.2023.
//

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
