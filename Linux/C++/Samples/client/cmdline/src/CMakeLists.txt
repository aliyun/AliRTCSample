cmake_minimum_required(VERSION 3.5)

set(CMAKE_VERBOSE_MAKEFILE ON)

# set(CMAKE_C_COMPILER "/usr/bin/gcc")
# set(CMAKE_CXX_COMPILER "/usr/bin/g++")

project(onertc_demo)

set(DEPS_LIB_DIR ${CMAKE_BINARY_DIR}/lib)
set(DINGRTC_SDK_PATH ${PROJECT_SOURCE_DIR}/../../../..)
set(DINGRTC_SDK_INCLUDE_PATH ${DINGRTC_SDK_PATH}/api)
set(DINGRTC_SDK_LIB_PATH ${DINGRTC_SDK_PATH}/lib/linux/${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE})
set(OWT_DEPS_LIB_PATH ${PROJECT_SOURCE_DIR}/../../../../thirdparty/owt-client-native/lib/linux/${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE})

set(TINY_HTTP_PATH ${PROJECT_SOURCE_DIR}/../../../../thirdparty/tinyhttp)
set(CURL_PATH ${PROJECT_SOURCE_DIR}/../../../../thirdparty/curl)
set(CMAKE_SKIP_RPATH TRUE)


message(STATUS "DEPS_LIB_DIR: ${DEPS_LIB_DIR}")
message(STATUS "DINGRTC_SDK_INCLUDE_PATH: ${DINGRTC_SDK_INCLUDE_PATH}")
message(STATUS "DINGRTC_SDK_LIB_PATH: ${DINGRTC_SDK_LIB_PATH}")
message(STATUS "OWT_DEPS_LIB_PATH: ${OWT_DEPS_LIB_PATH}")

include_directories(${DINGRTC_SDK_INCLUDE_PATH}
                    ${PROJECT_SOURCE_DIR}/../../../Common/ThirdParty
                    ${TINY_HTTP_PATH}/include
                    ${PROJECT_SOURCE_DIR}/../..)
link_directories(${DEPS_LIB_DIR}
                 ${TINY_HTTP_PATH}/lib/linux/${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE}
                 ${CURL_PATH}/libs/linux/${CMAKE_SYSTEM_PROCESSOR}
                 ${OWT_DEPS_LIB_PATH})

file(GLOB SOURCE_FILES ${PROJECT_SOURCE_DIR}/*.cc)
file(GLOB TLOG_SOURCE_FILES ${PROJECT_SOURCE_DIR}/../../tlog/*.cc)
file(GLOB MONGOOSE_SOURCE_FILES ${PROJECT_SOURCE_DIR}/../../mongoose/*.c)

set(TARGET "onertc_demo")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-undefined -Wl,-rpath=\$ORIGIN/lib -Wl,--disable-new-dtags")

add_executable(${TARGET} ${MONGOOSE_SOURCE_FILES} ${TLOG_SOURCE_FILES} ${SOURCE_FILES})

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    set(DINGRTC DingRTCd)
else()
    set(DINGRTC DingRTC)
endif()
target_link_libraries(${TARGET} TinyHttp curl ssl crypto ${DINGRTC} pthread dl)

# copy dependency libraries before build
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dummy
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DEPS_LIB_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        ${DINGRTC_SDK_LIB_PATH}/lib${DINGRTC}.so 
        ${DEPS_LIB_DIR}/lib${DINGRTC}.so
    COMMENT "Copying dependency libraries"
)
add_custom_target(CopyDeps ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/dummy # Making sure the custom command runs
)
add_dependencies(${TARGET} CopyDeps)
