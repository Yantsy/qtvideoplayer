cmake_minimum_required(VERSION 3.20)

project(qtvplayer)


# Qt相关
set(CMAKE_AUTOUIC ON)
# Qt's Meta-Object Compiler
set(CMAKE_AUTOMOC ON)
# Qt资源系统
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 COMPONENTS Quick Widgets Core Svg SvgWidgets Multimedia MultimediaWidgets OpenGL OpenGLWidgets REQUIRED)

set(FFMPEG_INCLUDE_DIR "/usr/local/ffmpeg/include")
set(FFMPEG_LIB_DIR "/usr/local/ffmpeg/lib")

include_directories(${FFMPEG_INCLUDE_DIR})
link_directories(${FFMPEG_LIB_DIR})

qt_standard_project_setup(REQUIRES 6.5)

add_subdirectory(src)

set(QT_QML_GENERATE_QMLS_INIT ON)

