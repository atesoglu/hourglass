cmake_minimum_required(VERSION 4.1)
project(hourglass)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)

add_executable(hourglass
        main.cpp
        src/mainwindow.cpp
        include/mainwindow.h
        src/timerengine.cpp
        include/timerengine.h
        src/animatedbackgroundwidget.cpp
        include/animatedbackgroundwidget.h
        src/systemtraymanager.cpp
        include/systemtraymanager.h
        src/soundplayer.cpp
        include/soundplayer.h
        resources.qrc
)

target_include_directories(hourglass PRIVATE include)

target_link_libraries(hourglass
        Qt6::Widgets
        Qt6::Multimedia
)