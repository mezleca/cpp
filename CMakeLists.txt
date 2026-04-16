cmake_minimum_required(VERSION 3.16)

# gcc on windows
if(WIN32 AND NOT DEFINED CMAKE_C_COMPILER AND NOT DEFINED CMAKE_CXX_COMPILER)
    set(CMAKE_C_COMPILER "gcc")
    set(CMAKE_CXX_COMPILER "g++")
endif()

# clang on linux
if (UNIX AND NOT DEFINED CMAKE_C_COMPILER AND NOT DEFINED CMAKE_CXX_COMPILER)
    set(CMAKE_C_COMPILER /usr/bin/clang)
    set(CMAKE_CXX_COMPILER /usr/bin/clang++)
endif()

project(template VERSION 3.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# fallback to Debug
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(WIN32)
    # force static build on windows
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
endif()

# get source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "src/*.hpp")

file(GLOB_RECURSE QML_FILES CONFIGURE_DEPENDS
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "src/qt/qml/*.qml"
)

set(REALM_CPP_NO_TESTS ON CACHE BOOL "" FORCE)

# something related to url prefixes?
# not that important (i think) and also requires meson
set(CPR_CURL_USE_LIBPSL FALSE)
set(CPR_CURL_USE_LIBPSL FALSE CACHE BOOL "" FORCE)
set(CPR_USE_SYSTEM_CURL ON CACHE BOOL "" FORCE)

find_package(CURL REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

# add dependencies
add_subdirectory(vendor/fmt)
add_subdirectory(vendor/cpr)
add_subdirectory(vendor/magic_enum)
add_subdirectory(vendor/json)
add_subdirectory(vendor/miniz)

include(FetchContent)

# download realm-cpp
if(WIN32)
    FetchContent_Declare(
        cpprealm
        URL "https://github.com/mezleca/rel-builds/releases/download/realm-cpp-2.2.0/realm-cpp-2.2.0-x64-windows-mingw-static.tar.gz"
    )
else()
    FetchContent_Declare(
        cpprealm
        URL "https://github.com/mezleca/rel-builds/releases/download/realm-cpp-2.2.0/realm-cpp-2.2.0-x64-linux-clang-shared.tar.gz"
    )
endif()

FetchContent_MakeAvailable(cpprealm)

qt_standard_project_setup()

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

set_source_files_properties(src/qt/qml/Theme.qml
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE
)

# skip qml compilation on debug (we use hot reload anyways)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_target(qml_files SOURCES ${QML_FILES})
endif()

qt_add_qml_module(${PROJECT_NAME}
    URI "osu_stuff"
    VERSION 3.0
    QML_FILES ${QML_FILES}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:QML_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/src/qt/qml">
    $<$<CONFIG:Debug>:QML_DISABLE_DISK_CACHE=1>
)

if(TARGET cpr)
    target_compile_options(cpr PRIVATE
        $<$<CXX_COMPILER_ID:Clang>:-Wno-error>
        $<$<CXX_COMPILER_ID:Clang>:-Wno-padded>
        $<$<CXX_COMPILER_ID:Clang>:-Wno-unique-object-duplication>
    )
endif()

if(DEFINED OUTPUT_NAME)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME ${OUTPUT_NAME}
        WIN32_EXECUTABLE ON
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    CURL::libcurl
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    fmt::fmt
    cpprealm
    nlohmann_json
    cpr
    miniz
    magic_enum
)

target_link_directories(${PROJECT_NAME} PRIVATE
    # prebuilts
    ${cpprealm_SOURCE_DIR}/lib
)

target_include_directories(${PROJECT_NAME} PRIVATE
    # prebuilts
    ${cpprealm_SOURCE_DIR}/include

    # other
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/...
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qt
)

add_custom_target(copy_compile_commands ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_SOURCE_DIR}/compile_commands.json
)
