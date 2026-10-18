#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QCoreApplication>

#include "cpu_monitor.h"
#include "memory_monitor.h"
#include "disk_monitor.h"
#include "gpu_monitor.h"
#include "network_monitor.h"
#include "process_monitor.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QGuiApplication::setApplicationName("LinuxSystemMonitor");

    QQmlApplicationEngine engine;
    CpuMonitor cpuMonitor;
    MemoryMonitor memoryMonitor;
    DiskMonitor diskMonitor;
    GpuMonitor gpuMonitor;
    NetworkMonitor networkMonitor;
    ProcessMonitor processMonitor;
    

    engine.rootContext()->setContextProperty(
        "cpuMonitor", &cpuMonitor);

    engine.rootContext()->setContextProperty(
        "memoryMonitor", &memoryMonitor);

    engine.rootContext()->setContextProperty(
        "diskMonitor", &diskMonitor);

    engine.rootContext()->setContextProperty(
        "gpuMonitor", &gpuMonitor);
    
    engine.rootContext()->setContextProperty(
        "networkMonitor", &networkMonitor);

    engine.rootContext()->setContextProperty(
        "processMonitor", &processMonitor);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection
    );

    engine.loadFromModule("LinuxSystemMonitor", "Main");

    return app.exec();
}