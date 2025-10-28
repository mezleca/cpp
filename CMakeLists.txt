cmake_minimum_required(VERSION 3.16)
project(test VERSION 0.0.1)

# fallback to Release mode
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# get source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB IMGUI_SOURCES "vendor/imgui/*.cpp")

set(IMGUI_BACKEND_SOURCES 
    "vendor/imgui/backends/imgui_impl_glfw.cpp"
    "vendor/imgui/backends/imgui_impl_opengl3.cpp"
)


# use c++ 17
set(CMAKE_CXX_STANDARD 17)

# realm options
set(REALM_NO_TESTS TRUE)
set(REALM_BUILD_LIB_ONLY TRUE)
set(REALM_NO_TESTS TRUE CACHE BOOL "" FORCE)
set(REALM_CPP_NO_TESTS TRUE CACHE BOOL "" FORCE)

find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME} 
    ${SOURCES}
    ${IMGUI_SOURCES}
    ${IMGUI_BACKEND_SOURCES}
)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    find_package(X11 REQUIRED COMPONENTS Xi)
    target_include_directories(${PROJECT_NAME} PUBLIC ${X11_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${X11_LIBRARIES}
        ${X11_Xi_LIB}
    )
endif()

# add dependencies
add_subdirectory(vendor/realm-cpp)
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glad)

target_link_libraries(${PROJECT_NAME} PRIVATE cpprealm glad glfw OpenGL::GL)
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/realm-cpp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include
)
