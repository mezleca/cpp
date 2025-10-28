cmake_minimum_required(VERSION 3.16)
project(test VERSION 0.0.1)

# fallback to Release mode
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)

# realm shit
set(REALM_NO_TESTS TRUE)
set(REALM_BUILD_LIB_ONLY TRUE)

# disable realm tests
set(REALM_NO_TESTS TRUE CACHE BOOL "" FORCE)
set(REALM_CPP_NO_TESTS TRUE CACHE BOOL "" FORCE)

add_executable(${PROJECT_NAME} src/main.cpp)

# add dependencies
add_subdirectory(vendor/realm-cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE cpprealm)
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/realm-cpp/include
)
