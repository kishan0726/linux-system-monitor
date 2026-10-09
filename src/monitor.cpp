
#include "monitor.h"

#include <QFile>
#include <QRegularExpression>
#include <QTimer>

#include <algorithm>

namespace
{
    double memoryValueGB(const QString &key)
    {
        QFile file("/proc/meminfo");

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return 0.0;

        const QString contents = QString::fromUtf8(file.readAll());
        const QRegularExpression expression(
            "^" + key + R"(:\s+(\d+)\s+kB)",
            QRegularExpression::MultilineOption
        );

        const auto match = expression.match(contents);

        if (!match.hasMatch())
            return 0.0;

        return match.captured(1).toDouble() / (1024.0 * 1024.0);
    }
}

Monitor::Monitor(QObject *parent)
    : QObject(parent)
{
    connect(&timer, &QTimer::timeout,
            this, &Monitor::collectMetrics);

    timer.start(1000);
    collectMetrics();
}

void Monitor::collectMetrics()
{
    const double totalRAM = memoryValueGB("MemTotal");
    const double availableRAM = memoryValueGB("MemAvailable");
    const double usedRAM = std::max(0.0, totalRAM - availableRAM);

    const double totalSwap = memoryValueGB("SwapTotal");
    const double freeSwap = memoryValueGB("SwapFree");
    const double usedSwap = std::max(0.0, totalSwap - freeSwap);

    QJsonObject metrics;

    // CPU sampling will be added next.
    metrics["cpu"] = 0.0;

    metrics["ramTotal"] = totalRAM;
    metrics["ramUsed"] = usedRAM;
    metrics["ramPercent"] =
        totalRAM > 0.0 ? usedRAM * 100.0 / totalRAM : 0.0;

    metrics["swapTotal"] = totalSwap;
    metrics["swapUsed"] = usedSwap;
    metrics["swapPercent"] =
        totalSwap > 0.0 ? usedSwap * 100.0 / totalSwap : 0.0;

    emit metricsUpdated(metrics);
}
