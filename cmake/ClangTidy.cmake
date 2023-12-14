function(clang_tidy TARGET)
    find_program(CLANG_TIDY "clang-tidy")

    get_target_property(TARGET_SOURCES ${TARGET} SOURCES)

    set(TIDY_TASK_DEPS)
    set(TIDYFIX_TASK_DEPS)

    foreach (TARGET_SOURCE ${TARGET_SOURCES})
        get_filename_component(SOURCE_EXT ${TARGET_SOURCE} EXT)
        string(REGEX REPLACE "[ \t\r\n/\\]+" "_" TASK_KEY ${TARGET_SOURCE})
        if ("${SOURCE_EXT}" STREQUAL ".cpp")
            add_custom_target("${TASK_KEY}_tidy"
                    COMMAND ${CLANG_TIDY} -p "${CMAKE_BINARY_DIR}" ${CMAKE_SOURCE_DIR}/${TARGET_SOURCE}
            )
            add_custom_target("${TASK_KEY}_tidy_fix"
                    COMMAND ${CLANG_TIDY} -p "${CMAKE_BINARY_DIR}" ${CMAKE_SOURCE_DIR}/${TARGET_SOURCE} --fix
            )
            list(APPEND TIDY_TASK_DEPS "${TASK_KEY}_tidy")
            list(APPEND TIDYFIX_TASK_DEPS "${TASK_KEY}_tidy_fix")
        endif ()
    endforeach ()

    list(JOIN COMMAND_LIST " && " COMMANDS)

    add_custom_target("${TARGET}_tidy"
            DEPENDS ${TIDY_TASK_DEPS}
    )
    add_custom_target("${TARGET}_tidy_fix"
            DEPENDS ${TIDYFIX_TASK_DEPS}
    )
endfunction()