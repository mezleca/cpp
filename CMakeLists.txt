cmake_minimum_required(VERSION 3.16)

# clang-cl on window
if(WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_CXX_COMPILER "C:/Program Files/LLVM/bin/clang-cl.exe")
    set(CMAKE_C_COMPILER "C:/Program Files/LLVM/bin/clang-cl.exe")
    set(CMAKE_LINKER "C:/Program Files/LLVM/bin/lld-link.exe")
    set(CMAKE_MT "C:/Program Files/LLVM/bin/llvm-mt.exe")
    set(CMAKE_RC_COMPILER "C:/Program Files/LLVM/bin/llvm-rc.exe")
endif()

# clang on linux
if (LINUX AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_C_COMPILER /usr/bin/clang)
    set(CMAKE_CXX_COMPILER /usr/bin/clang++)
endif()

project(template VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# fallback to Release mode
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# get source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "src/*.hpp")


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

if(WIN32)
    # force static on windows
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

    target_compile_options(${PROJECT_NAME} PRIVATE /EHsc /MT /W4 /WX-)

    set(CMAKE_CXX_FLAGS_RELEASE "/O2")
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi")
endif()

if (LINUX)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
endif()

if(TARGET cpr)
    target_compile_options(cpr PRIVATE
        $<$<CXX_COMPILER_ID:Clang>:-Wno-error>
        $<$<CXX_COMPILER_ID:Clang>:-Wno-padded>
        $<$<CXX_COMPILER_ID:Clang>:-Wno-unique-object-duplication>
    )
endif()

if(DEFINED OUTPUT_NAME)
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${OUTPUT_NAME})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    fmt::fmt
    nlohmann_json
    cpr
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/...
)

# copy compile_commands.json
add_custom_target(copy_compile_commands ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_SOURCE_DIR}/compile_commands.json
)
