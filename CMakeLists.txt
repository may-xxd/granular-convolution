cmake_minimum_required(VERSION 3.20)

PROJECT(granular-convolution)

set (BUILD_SHARED OFF FORCE)
set (BUILD_STATIC ON FORCE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB FFT_CONVOLVER_SOURCES ${PROJECT_SOURCE_DIR}/deps/FFTConvolver/*.cpp)
file(GLOB HILBERT_CURVES_SOURCES ${PROJECT_SOURCE_DIR}/deps/hilbert_curves/*.cpp)

add_executable(granular-convolution ${SOURCES} ${FFT_CONVOLVER_SOURCES} ${HILBERT_CURVES_SOURCES})

target_include_directories(granular-convolution PRIVATE ${PROJECT_SOURCE_DIR}/deps/FFTConvolver ${PROJECT_SOURCE_DIR}/deps/hilbert_curves)

# Enable all warnings
if (MSVC)
  target_compile_options(granular-convolution PRIVATE /W4 /WX)
else()
  target_compile_options(granular-convolution PRIVATE -Wall -Wextra -pedantic -Werror -Wno-missing-braces -Wno-unused-function -g)
endif()

