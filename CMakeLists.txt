cmake_minimum_required(VERSION 3.27)
project(dhset CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(nlohmann_json 3.10.5 REQUIRED)

add_library(dhset SHARED
        src/vendor/redismodule.h
        src/module.cpp
        src/command/DiffHSet.cpp
        src/command/DiffHSet.hpp
        src/util/util.cpp
        src/util/util.hpp
        src/exec/ValueGetter.cpp
        src/exec/ValueGetter.hpp
        src/values.hpp
        src/config.hpp
        src/config.cpp
        src/config/NotificationMode.hpp
        src/config/Serializer.hpp
)

target_include_directories(dhset PRIVATE src/)
target_link_libraries(dhset PRIVATE nlohmann_json::nlohmann_json)

install(TARGETS dhset
        LIBRARY
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
)