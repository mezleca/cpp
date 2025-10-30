cmake_minimum_required(VERSION 3.16)
project(template VERSION 0.0.1)

# vscodium uses clang
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# fallback to Release mode
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# get source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "src/*.hpp")

if(MSVC)
    add_compile_options(/MP /MT)
endif()

# copy compile_commands so clang show files from include dir
# this sucks but idk any other way to make ts work
if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
        ${CMAKE_CURRENT_SOURCE_DIR}/build/compile_commands.json
    )
endif()

set(CMAKE_CXX_STANDARD 17)
set(BUILD_TESTING OFF CACHE BOOL "Build tests" FORCE)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# nuh uh
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# realm options
set(REALM_NO_TESTS TRUE)
set(REALM_BUILD_LIB_ONLY TRUE)
set(REALM_NO_TESTS TRUE CACHE BOOL "" FORCE)
set(REALM_CPP_NO_TESTS TRUE CACHE BOOL "" FORCE)

# something related to url prefixes?
# not that important (i think) and also requires meson
set(CPR_CURL_USE_LIBPSL FALSE)
set(CPR_CURL_USE_LIBPSL FALSE CACHE BOOL "" FORCE)

# add dependencies
add_subdirectory(vendor/fmt)
add_subdirectory(vendor/cpr)
add_subdirectory(vendor/json)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

if(DEFINED OUTPUT_NAME)
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${OUTPUT_NAME})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    fmt::fmt
    nlohmann_json
    cpr
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/fmt/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/cpr/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/json/include
)

add_custom_target(copy_compile_commands ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
    DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
    COMMENT "copying compile_commands.json to project root"
)
