
cmake_minimum_required(VERSION 3.21)

project(LinuxSystemMonitor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    WebEngineWidgets
    WebChannel
)

add_executable(linux-system-monitor
    src/main.cpp
    src/monitor.cpp
    include/monitor.h
)

target_include_directories(linux-system-monitor PRIVATE
    include
)

target_link_libraries(linux-system-monitor PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::WebChannel
)
