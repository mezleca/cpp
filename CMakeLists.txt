cmake_minimum_required(VERSION 3.16)

# gcc on windows
if(WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE AND NOT DEFINED CMAKE_C_COMPILER AND NOT DEFINED CMAKE_CXX_COMPILER)
    set(CMAKE_C_COMPILER "gcc")
    set(CMAKE_CXX_COMPILER "g++")
endif()

# clang on linux
if (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT DEFINED CMAKE_TOOLCHAIN_FILE AND NOT DEFINED CMAKE_C_COMPILER AND NOT DEFINED CMAKE_CXX_COMPILER)
    set(CMAKE_C_COMPILER /usr/bin/clang)
    set(CMAKE_CXX_COMPILER /usr/bin/clang++)
endif()

project(template VERSION 3.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# fallback to Release mode
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
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

if(WIN32)
    # force static on windows
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
endif()

if (LINUX)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
endif()

# add dependencies
add_subdirectory(vendor/fmt)
add_subdirectory(vendor/cpr)
add_subdirectory(vendor/magic_enum)
add_subdirectory(vendor/json)
add_subdirectory(vendor/miniz)
add_subdirectory(vendor/realm-cpp)

include(FetchContent)

FetchContent_Declare(
    Boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.87.0/boost-1.87.0-cmake.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP ON
)

set(BOOST_INCLUDE_LIBRARIES locale)
FetchContent_MakeAvailable(Boost)

if(TARGET cpprealm)
    target_compile_options(cpprealm PRIVATE
        $<$<CXX_COMPILER_ID:Clang>:-Wno-deprecated-literal-operator>
        $<$<CXX_COMPILER_ID:Clang>:-Wno-unused-private-field>
    )
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

qt_standard_project_setup()

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

set_source_files_properties(src/qt/qml/Theme.qml
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(${PROJECT_NAME}
    URI "osu_stuff"
    VERSION 3.0
    QML_FILES ${QML_FILES}
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
    fmt::fmt
    nlohmann_json
    cpr
    cpprealm
    miniz
    magic_enum
    Boost::locale
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/...
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qt
)

add_custom_target(copy_compile_commands ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_SOURCE_DIR}/compile_commands.json
)
