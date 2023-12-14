cmake_minimum_required(VERSION 3.24)
project(dhset CXX)

cmake_policy(SET CMP0135 NEW)

include(cmake/ClangTidy.cmake)
include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-missing-field-initializers")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(
        NLOHMANN_JSON_VERSION "3.11.3"
        CACHE STRING "nlohmann::json version"
)
set(
        NLOHMANN_JSON_SHA256 "d6c65aca6b1ed68e7a182f4757257b107ae403032760ed6ef121c9d55e81757d"
        CACHE STRING "nlohmann::json version"
)

FetchContent_Declare(json
        URL https://github.com/nlohmann/json/releases/download/v${NLOHMANN_JSON_VERSION}/json.tar.xz
        URL_HASH SHA256=${NLOHMANN_JSON_SHA256}
)
FetchContent_MakeAvailable(json)

add_library(dhset SHARED)

target_sources(dhset PRIVATE
        src/module.cpp
        src/command/DiffHSet.cpp
        src/command/DiffHSet.hpp
        src/util/util.cpp
        src/util/util.hpp
        src/exec/ValueGetter.cpp
        src/exec/ValueGetter.hpp
        src/values.hpp
        src/config/ModuleState.hpp
        src/config/ModuleState.cpp
        src/config/NotificationMode.hpp
        src/config/NotificationMode.cpp
        src/config/Serializer.hpp
        src/config/Serializer.cpp
        src/config/KeyPattern.cpp
        src/config/KeyPattern.hpp
        src/config/ShrinkStrategy.hpp
        src/config/ShrinkStrategy.cpp
        src/config/PrimitiveParams.hpp
        src/config/PrimitiveParams.cpp
        src/command/CommandFilter.cpp
        src/command/CommandFilter.hpp
        src/config/DelegateTo.cpp
        src/config/DelegateTo.hpp
        src/util/ranges.hpp
        src/command/DHSetGetCache.cpp
        src/command/DHSetGetCache.hpp
)

clang_tidy(dhset)

target_link_libraries(dhset PRIVATE nlohmann_json::nlohmann_json)

target_include_directories(dhset PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_include_directories(dhset SYSTEM PRIVATE "${PROJECT_SOURCE_DIR}/include")

set(
        OUTPUT_LIBC_SUFFIX "unknown"
        CACHE "Library suffix to specify target libc distribution"
)

set_target_properties(dhset
        PROPERTIES LIBRARY_OUTPUT_NAME "dhset-${CMAKE_SYSTEM_PROCESSOR}-${OUTPUT_LIBC_SUFFIX}"
)

target_compile_definitions(dhset
        PRIVATE REDISMODULE_API=extern\ "C"
)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_custom_command(
            TARGET dhset
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy "${PROJECT_BINARY_DIR}/libdhset.so" "${PROJECT_SOURCE_DIR}/debug/"
    )

    if (DEFINED DEVENV_PREPARE_REDIS)
        set(
                DEVENV_REDIS_VERSION "7.2.3"
                CACHE STRING "Redis version"
        )
        set(
                DEVENV_REDIS_SHA256 afd656dbc18a886f9a1cc08a550bf5eb89de0d431e713eba3ae243391fb008a6
                CACHE STRING "SHA256 of redis-${DEVENV_REDIS_VERSION}.tar.gz"
        )

        find_program(MAKE_COMMAND make)

        file(DOWNLOAD
                "https://github.com/redis/redis/archive/refs/tags/${DEVENV_REDIS_VERSION}.tar.gz"
                "${PROJECT_SOURCE_DIR}/misc/redis.tar.gz"
                SHOW_PROGRESS
                EXPECTED_HASH SHA256=${DEVENV_REDIS_SHA256}
        )
        execute_process(COMMAND
                ${CMAKE_COMMAND} -E tar xzvf redis.tar.gz
                WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/misc"
        )
        execute_process(COMMAND
                ${MAKE_COMMAND} noopt
                WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/misc/redis-${DEVENV_REDIS_VERSION}"
        )
        execute_process(COMMAND
                ${MAKE_COMMAND} install "PREFIX=${PROJECT_SOURCE_DIR}/debug/redis"
                WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/misc/redis-${DEVENV_REDIS_VERSION}"
        )
    endif ()
endif ()

install(TARGETS dhset
        LIBRARY
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
)