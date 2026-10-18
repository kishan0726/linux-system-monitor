
cmake_minimum_required(VERSION 3.21)

project(LinuxSystemMonitor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(linux-system-monitor
    src/main.cpp
    src/cpu_monitor.cpp
    include/cpu_monitor.h
    src/memory_monitor.cpp
    include/memory_monitor.h
    include/disk_monitor.h
    src/disk_monitor.cpp
    include/gpu_monitor.h
    src/gpu_monitor.cpp
    include/network_monitor.h
    src/network_monitor.cpp
    include/process_monitor.h
    src/process_monitor.cpp
)

qt_add_qml_module(linux-system-monitor
    URI LinuxSystemMonitor
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/components/ResourceChart.qml
)

target_link_libraries(linux-system-monitor PRIVATE
    Qt6::Quick
)

target_include_directories(linux-system-monitor
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
