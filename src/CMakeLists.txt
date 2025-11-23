qt_add_executable(${PROJECT_NAME}
    ../src/home.cc
    ../src/home.h
)

qt_add_resources({${PROJECT_NAME} ../resources.qrc})

target_sources(${PROJECT_NAME} PRIVATE ../resources.qrc)

    
target_link_libraries(${PROJECT_NAME}  PUBLIC Qt6::Core Qt6::Widgets Qt6::Quick Qt6::Svg Qt6::SvgWidgets)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
