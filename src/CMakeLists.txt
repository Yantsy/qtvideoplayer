qt_add_executable(${PROJECT_NAME}
    ../src/home.cc
    ../src/home.h
    ../src/main.cc
../src/rbvideowidget.cc
    ../src/rbvideowidget.h    
    ../src/polygon.cc
    ../src/polygon.h
    ../src/yslider.cc
    ../src/yslider.h
    ../src/playlist.cc
    ../src/playlist.h
    ../src/label.h
    ../src/VideoWidget.h
    ../src/VideoWidget.cc
)

qt_add_resources({${PROJECT_NAME} ../resources.qrc})

target_sources(${PROJECT_NAME} PRIVATE ../resources.qrc)

    
target_link_libraries(${PROJECT_NAME}  PUBLIC Qt6::Core Qt6::Widgets Qt6::Quick Qt6::Svg Qt6::SvgWidgets )

target_link_libraries(${PROJECT_NAME}  PRIVATE Qt6::Multimedia Qt6::MultimediaWidgets)


set(CMAKE_POSITION_INDEPENDENT_CODE ON)
