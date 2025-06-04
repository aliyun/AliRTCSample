cmake_minimum_required(VERSION 3.5)

project(ServerPullDemo VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

add_executable(server_pull_demo
    linux_server_pull_demo.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/../../SDK/api)
set(SDK_LIB_DIR ${PROJECT_SOURCE_DIR}/../../SDK/lib/x86_64)
link_directories(${SDK_LIB_DIR})
find_library(DING_RTC_LIB NAMES DingRTC PATHS ${SDK_LIB_DIR} NO_DEFAULT_PATH)
if (NOT DING_RTC_LIB)
    message(FATAL_ERROR "Cannot find libDingRTC.so in SDK/lib/x86_64")
else()
    message(STATUS "Found libDingRTC at: ${DING_RTC_LIB}")
endif()
find_library(FFMPEG_LIB NAMES ffmpeg PATHS ${SDK_LIB_DIR} NO_DEFAULT_PATH)
if (NOT FFMPEG_LIB)
    message(FATAL_ERROR "Cannot find libffmpeg.so in SDK/lib/x86_64")
else()
    message(STATUS "Found libffmpeg at: ${FFMPEG_LIB}")
endif()

target_link_libraries(server_pull_demo PRIVATE ${DING_RTC_LIB} ${FFMPEG_LIB})
