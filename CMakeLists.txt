cmake_minimum_required(VERSION 3.22)
project(dhset CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(nlohmann_json 3.10.5 REQUIRED)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-missing-field-initializers")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_library(dhset SHARED
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


target_link_libraries(dhset PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(dhset PRIVATE "${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/include")

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