cmake_minimum_required(VERSION 3.21)
project(NodeWatchDesktop LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network HttpServer)

qt_standard_project_setup()

qt_add_executable(NodeWatchDesktop
    src/main.cpp
    src/MainWindow.cpp
    src/HttpServer.cpp
    src/TelemetryEntry.cpp
    include/MainWindow.h
    include/HttpServer.h
    include/TelemetryEntry.h
)

target_include_directories(NodeWatchDesktop PRIVATE include)

target_link_libraries(NodeWatchDesktop PRIVATE
    Qt6::Widgets
    Qt6::Network
    Qt6::HttpServer
)