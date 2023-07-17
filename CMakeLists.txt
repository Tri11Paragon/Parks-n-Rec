cmake_minimum_required(VERSION 3.0)
project(parksnrec VERSION 0.0.1)

option(ENABLE_ADDRSAN "Enable the address sanitizer" OFF)
option(ENABLE_UBSAN "Enable the ub sanitizer" OFF)
option(ENABLE_TSAN "Enable the thread data race sanitizer" OFF)

set(CMAKE_CXX_STANDARD 20)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL)

add_subdirectory(libs/BLT)
add_subdirectory(libs/glfw/glfw-3.3.8)

set(OLD_CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE})
set(CMAKE_BUILD_TYPE "Release")
set(ASSIMP_INSTALL OFF)
set(ASSIMP_WARNINGS_AS_ERRORS OFF)
set(ASSIMP_BUILD_TESTS OFF)

add_subdirectory(libs/assimp)

set(CMAKE_BUILD_TYPE ${OLD_CMAKE_BUILD_TYPE})

include_directories(include/)

file(GLOB_RECURSE source_files src/*.cpp)

add_executable(parksnrec ${source_files})

target_link_libraries(parksnrec glfw)
target_link_libraries(parksnrec BLT)
target_link_libraries(parksnrec OpenGL)
target_link_libraries(parksnrec assimp)
target_compile_options(parksnrec PRIVATE -Wall -Wextra -Wpedantic)

if (${ENABLE_ADDRSAN} MATCHES ON)
    target_compile_options(parksnrec PRIVATE -fsanitize=address)
    target_link_options(parksnrec PRIVATE -fsanitize=address)
endif ()

if (${ENABLE_UBSAN} MATCHES ON)
    target_compile_options(parksnrec PRIVATE -fsanitize=undefined)
    target_link_options(parksnrec PRIVATE -fsanitize=undefined)
endif ()

if (${ENABLE_TSAN} MATCHES ON)
    target_compile_options(parksnrec PRIVATE -fsanitize=thread)
    target_link_options(parksnrec PRIVATE -fsanitize=thread)
endif ()
