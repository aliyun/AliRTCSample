cmake_minimum_required(VERSION 3.5)

project(OneRTCQtDemo VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_SKIP_RPATH TRUE)

set(DEPS_LIB_DIR ${CMAKE_BINARY_DIR}/lib)
set(DINGRTC_SDK_PATH ${PROJECT_SOURCE_DIR}/../../../..)
set(DINGRTC_SDK_INCLUDE_PATH ${DINGRTC_SDK_PATH}/api)
set(DINGRTC_SDK_LIB_PATH ${DINGRTC_SDK_PATH}/lib/linux/${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE})
set(OWT_DEPS_LIB_PATH ${PROJECT_SOURCE_DIR}/../../../../thirdparty/owt-client-native/lib/linux/${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE})

set(COMMON_SOURCE_PATH ${PROJECT_SOURCE_DIR}/../../common)
set(TLOG_SOURCE_PATH ${PROJECT_SOURCE_DIR}/../../tlog)

file(GLOB COMMON_SOURCES ${COMMON_SOURCE_PATH}/*.cc ${COMMON_SOURCE_PATH}/*.h)
file(GLOB TLOG_SOURCES ${TLOG_SOURCE_PATH}/*.cc ${TLOG_SOURCE_PATH}/*.h)

set(OPENSSL_PATH /home/alidev/Qt/Tools/OpenSSLv3/src)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

include_directories(${DINGRTC_SDK_INCLUDE_PATH}
                    ${COMMON_SOURCE_PATH}
                    ${TLOG_SOURCE_PATH}/..)
                    # ${TINY_HTTP_PATH}/include
                    # ${PROJECT_SOURCE_DIR}/../../../Common/ThirdParty/nlohmann)
link_directories(${DEPS_LIB_DIR})
                 # ${TINY_HTTP_PATH}/lib/linux/${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE})


set(PROJECT_SOURCES
    main.cpp
    MainWindow.cpp
    MainWindow.h
    MainWindow.ui
    MeetingWindow.cpp
    MeetingWindow.h
    MeetingWindow.ui
    VideoSettingsWidget.cpp
    VideoSettingsWidget.h
    VideoSettingsWidget.ui
    AudioSettingsWidget.cpp
    AudioSettingsWidget.h
    AudioSettingsWidget.ui
    ExternalSourceWidget.cpp
    ExternalSourceWidget.h
    ExternalSourceWidget.ui
    AVFrameObserverWidget.cpp
    AVFrameObserverWidget.h
    AVFrameObserverWidget.ui
    GeneralSettingsWidget.cpp
    GeneralSettingsWidget.h
    GeneralSettingsWidget.ui
    AppSettings.cpp
    AppSettings.h
    Utils.cpp
    Utils.h
    RtcEngineObject.cpp
    RtcEngineObject.h
    HttpClient.cpp
    HttpClient.h
    CellWidget.cpp
    CellWidget.h
    SettingsWindow.cpp
    SettingsWindow.h
    DeviceInfo.h
)

#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-undefined -Wl,--disable-new-dtags")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-undefined -Wl,-rpath=\$ORIGIN/lib -Wl,--disable-new-dtags")

# if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
#     qt_add_executable(OneRTCQtDemo
#         MANUAL_FINALIZATION
#         ${PROJECT_SOURCES}
#         GridConfViewControls.ui
#     )
# # Define target properties for Android with Qt 6 as:
# #    set_property(TARGET OneRTCQtDemo APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
# #                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# # For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
# else()
#     add_executable(OneRTCQtDemo
#         ${PROJECT_SOURCES}
#     )
# endif()

add_executable(OneRTCQtDemo
    resources.qrc
    ${PROJECT_SOURCES}
    ${COMMON_SOURCES}
    ${TLOG_SOURCES}
)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    set(DINGRTC DingRTCd)
else()
    set(DINGRTC DingRTC)
endif()
#set(DINGRTC DingRTC)

target_link_libraries(OneRTCQtDemo PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network ${DINGRTC} ffmpeg pthread)

# copy dependency libraries before build
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dummy
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DEPS_LIB_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
     ${DINGRTC_SDK_LIB_PATH}/lib${DINGRTC}.so
     ${DEPS_LIB_DIR}/lib${DINGRTC}.so
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
     ${OWT_DEPS_LIB_PATH}/libffmpeg.so
     ${DEPS_LIB_DIR}/libffmpeg.so
    COMMENT "Copying dependency libraries"
)
add_custom_target(CopyDeps ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/dummy # Making sure the custom command runs
)
add_dependencies(OneRTCQtDemo CopyDeps)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
# if(${QT_VERSION} VERSION_LESS 6.1.0)
#   set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.OneRTCQtDemo)
# endif()
# set_target_properties(OneRTCQtDemo PROPERTIES
#     ${BUNDLE_ID_OPTION}
#     MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#     MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
#     MACOSX_BUNDLE TRUE
#     WIN32_EXECUTABLE TRUE
# )

include(GNUInstallDirs)
install(TARGETS OneRTCQtDemo
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# if(QT_VERSION_MAJOR GREATER_EQUAL 6)
#     qt_finalize_executable(OneRTCQtDemo)
# endif()
