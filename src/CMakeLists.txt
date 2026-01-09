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
)

qt_add_resources({${PROJECT_NAME} ../resources.qrc})

target_sources(${PROJECT_NAME} PRIVATE ../resources.qrc)

    
target_link_libraries(${PROJECT_NAME}  PUBLIC Qt6::Core Qt6::Widgets Qt6::Quick Qt6::Svg Qt6::SvgWidgets Qt6::OpenGL Qt6::OpenGLWidgets)

target_link_libraries(${PROJECT_NAME}  PRIVATE Qt6::Multimedia Qt6::MultimediaWidgets)

target_link_libraries(${PROJECT_NAME} PUBLIC avformat avcodec avutil swscale swresample)

target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)

target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})


set(CMAKE_POSITION_INDEPENDENT_CODE ON)
