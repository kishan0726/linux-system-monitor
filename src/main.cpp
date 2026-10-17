#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QCoreApplication>

#include "cpu_monitor.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QGuiApplication::setApplicationName("LinuxSystemMonitor");

    QQmlApplicationEngine engine;
    CpuMonitor cpuMonitor;

    engine.rootContext()->setContextProperty(
        "cpuMonitor", &cpuMonitor);

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