cmake_minimum_required(VERSION 3.20)

project(cpp-shell VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)

add_executable(shell ${SOURCE_FILES})

target_include_directories(shell PRIVATE include)

target_link_libraries(shell PRIVATE readline)