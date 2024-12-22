cmake_minimum_required(VERSION 3.16)

project ("Client")
project ("Server")

add_subdirectory ("Client")
add_subdirectory ("Server")
add_subdirectory ("Utilities")

find_program(CLANG_FORMAT_EXECUTABLE NAMES clang-format)

if (CLANG_FORMAT_EXECUTABLE)
    message(STATUS "Found clang-format: ${CLANG_FORMAT_EXECUTABLE}")

    # Add a custom target to format all source files
    file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
        "${CMAKE_SOURCE_DIR}/*.cpp"
        "${CMAKE_SOURCE_DIR}/*.hpp"
    )

    add_custom_target(
        format
        COMMAND ${CLANG_FORMAT_EXECUTABLE} -i ${ALL_CXX_SOURCE_FILES}
        COMMENT "Running clang-format on source files"
    )

    add_custom_target(
        check-format
        COMMAND ${CLANG_FORMAT_EXECUTABLE} --dry-run --Werror ${ALL_CXX_SOURCE_FILES}
        COMMENT "Checking formatting without modifying files"
    )

else()
    message(WARNING "clang-format not found, skipping format target.")
endif()