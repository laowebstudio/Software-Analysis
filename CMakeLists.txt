cmake_minimum_required(VERSION 3.21)
project(SAPUDOMDesktop VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(SAPUDOM
    src/main.cpp
    src/MainWindow.h
    src/MainWindow.cpp
    src/ModelCanvas.h
    src/ModelCanvas.cpp
)

target_link_libraries(SAPUDOM PRIVATE Qt6::Widgets)

set_target_properties(SAPUDOM PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "SAPUDOM Structural Analysis"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.sapudom.structuralanalysis"
    MACOSX_BUNDLE_BUNDLE_VERSION "0.1.0"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1.0"
)
