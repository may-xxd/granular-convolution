cmake_minimum_required(VERSION 3.20)

PROJECT(granular-convolution)

set (BUILD_SHARED OFF FORCE)
set (BUILD_STATIC ON FORCE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(granular-convolution ${SOURCES})

# Enable all warnings
if (MSVC)
  target_compile_options(granular-convolution PRIVATE /W4 /WX)
else()
  target_compile_options(granular-convolution PRIVATE -Wall -Wextra -pedantic -Werror -Wno-missing-braces -Wno-unused-function)
endif()

