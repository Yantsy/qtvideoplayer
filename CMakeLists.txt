cmake_minimum_required(VERSION 3.20)

project(qtvplayer)


# Qt相关
set(CMAKE_AUTOUIC ON)
# Qt's Meta-Object Compiler
set(CMAKE_AUTOMOC ON)
# Qt资源系统
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 COMPONENTS Quick Widgets Core Svg SvgWidgets Multimedia MultimediaWidgets OpenGL OpenGLWidgets REQUIRED)

find_package(Eigen3 REQUIRED)

find_package(OpenCV REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2MIXER REQUIRED SDL2_mixer)


set(FFMPEG_INCLUDE_DIR "/usr/local/ffmpeg/include")
set(FFMPEG_LIB_DIR "/usr/local/ffmpeg/lib")

include_directories(${FFMPEG_INCLUDE_DIR})
link_directories(${FFMPEG_LIB_DIR})

qt_standard_project_setup(REQUIRES 6.5)


set(QT_QML_GENERATE_QMLS_INIT ON)

qt_add_executable(${PROJECT_NAME}
    ../src/home.cc
    ../src/home.h
    ../src/main.cc
../src/rbvideowidget.cc
    ../src/rbvideowidget.h    
    ../src/console.cc
    ../src/console.h
    ../src/yslider.cc
    ../src/yslider.h
    ../src/playlist.cc
    ../src/playlist.h
    ../src/label.h
    ../videoprocessing/glwidget.cc
    ../videoprocessing/glwidget.h
)
qt_add_executable(test 
../videoprocessing/test.cc
../videoprocessing/demuxer.h
../videoprocessing/decoder.h
../videoprocessing/glwidget.h
../videoprocessing/glwidget.cc
../videoprocessing/audiowidget.h
../videoprocessing/resampler.h
../videoprocessing/public.h
)
qt_add_resources({${PROJECT_NAME} ../resources.qrc})

target_sources(${PROJECT_NAME} PRIVATE ../resources.qrc)

qt_add_resources({test ../resources.qrc})

target_sources(test PRIVATE ../resources.qrc)

    
target_link_libraries(${PROJECT_NAME}  PUBLIC Qt6::Core Qt6::Widgets Qt6::Quick Qt6::Svg Qt6::SvgWidgets Qt6::OpenGL Qt6::OpenGLWidgets OpenGL::GL OpenGL::GLU)

target_link_libraries(test  PUBLIC Qt6::Core Qt6::Widgets Qt6::Quick Qt6::Svg Qt6::SvgWidgets Qt6::OpenGL Qt6::OpenGLWidgets OpenGL::GL OpenGL::GLU)

target_link_libraries(${PROJECT_NAME}  PRIVATE Qt6::Multimedia Qt6::MultimediaWidgets)
target_link_libraries(test  PRIVATE Qt6::Multimedia Qt6::MultimediaWidgets)
target_link_libraries(${PROJECT_NAME} PUBLIC avformat avcodec avutil swscale swresample)

target_link_libraries(test PUBLIC avformat avcodec avutil swscale swresample)

target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)

target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})



target_link_libraries(test PRIVATE
    ${SDL2_LIBRARIES}
    ${SDL2MIXER_LIBRARIES}
)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

target_precompile_headers(test PRIVATE videoprocessing/public.h)
