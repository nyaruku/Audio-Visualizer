cmake_minimum_required(VERSION 3.15)
project("AudioVisualizer" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

# Add Unicode definitions globally
add_definitions(-DUNICODE -D_UNICODE)

# Base directories
set(BASE_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/AudioVisualizer/includes")
set(BASE_LIB_PATH "${CMAKE_SOURCE_DIR}/AudioVisualizer/libs")

# Debug/Release-specific paths
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/AudioVisualizer/includes/Debug/")
    set(BUILD_LIB_PATH "${CMAKE_SOURCE_DIR}/AudioVisualizer/libs/Debug/")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(BUILD_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/AudioVisualizer/includes/Release/")
    set(BUILD_LIB_PATH "${CMAKE_SOURCE_DIR}/AudioVisualizer/libs/Release/")
endif()

# Get the DXSDK_DIR environment variable
set(DXSDK_DIR $ENV{DXSDK_DIR})

# Add the include directories for the project
include_directories(${BASE_INCLUDE_PATH} ${BUILD_INCLUDE_PATH} "${DXSDK_DIR}/Include" "${BASE_INCLUDE_PATH}/BASS" "${BASE_INCLUDE_PATH}/IMGUI-DX9")

# Add the library directories for linking
link_directories(${BASE_LIB_PATH} ${BUILD_LIB_PATH} "${DXSDK_DIR}/Lib/x86" "${BASE_LIB_PATH}/BASS")

# Libraries
link_libraries("bass.lib")
link_libraries("basscd.lib")
link_libraries("bassflac.lib")
link_libraries("basshls.lib")
link_libraries("bassopus.lib")
link_libraries("basswasapi.lib")
link_libraries("basswma.lib")
link_libraries("basswv.lib")
link_libraries("d3d9.lib")
link_libraries("d3dx9.lib")

add_subdirectory("AudioVisualizer")

# Debugging output for architecture
message(STATUS "#################################################")
message(STATUS "#################################################")
message(STATUS "CMAKE_SYSROOT: ${CMAKE_SYSROOT}")
message(STATUS "BASE_INCLUDE_PATH: ${BASE_INCLUDE_PATH}")
message(STATUS "BASE_LIB_PATH: ${BASE_LIB_PATH}")
message(STATUS "BUILD_INCLUDE_PATH: ${BUILD_INCLUDE_PATH}")
message(STATUS "BUILD_LIB_PATH: ${BUILD_LIB_PATH}")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "#################################################")
message(STATUS "#################################################")

