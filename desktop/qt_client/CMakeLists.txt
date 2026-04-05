cmake_minimum_required(VERSION 3.21)
project(NodeWatchDesktop LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network HttpServer Sql)

qt_standard_project_setup()

qt_add_executable(NodeWatchDesktop
    src/main.cpp
    src/MainWindow.cpp
    src/HttpServer.cpp
    src/TelemetryEntry.cpp
    src/TelemetryStore.cpp
    include/MainWindow.h
    include/HttpServer.h
    include/TelemetryEntry.h
    include/TelemetryStore.h
)

target_include_directories(NodeWatchDesktop PRIVATE include)

target_link_libraries(NodeWatchDesktop PRIVATE
    Qt6::Widgets
    Qt6::Network
    Qt6::HttpServer
    Qt6::Sql
)
