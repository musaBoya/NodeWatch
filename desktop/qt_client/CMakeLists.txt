cmake_minimum_required(VERSION 3.21)
project(NodeWatchDesktop LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network HttpServer Sql)

qt_standard_project_setup()

qt_add_executable(NodeWatchDesktop
    src/main.cpp
    src/config/AppSettings.cpp
    src/logging/logger.cpp
    src/ui/MainWindow.cpp
    src/server/HttpServer.cpp
    src/models/TelemetryEntry.cpp
    src/storage/TelemetryStore.cpp
    src/rules/TelemetryAlerts.cpp
    include/config/AppSettings.h
    include/logging/log_level.hpp
    include/logging/logger.hpp
    include/ui/MainWindow.h
    include/server/HttpServer.h
    include/models/TelemetryEntry.h
    include/storage/TelemetryStore.h
    include/rules/TelemetryAlerts.h
)

target_include_directories(NodeWatchDesktop PRIVATE include)

target_link_libraries(NodeWatchDesktop PRIVATE
    Qt6::Widgets
    Qt6::Network
    Qt6::HttpServer
    Qt6::Sql
)
