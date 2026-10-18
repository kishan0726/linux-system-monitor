
#include "gpu_monitor.h"

#include <QProcess>
#include <QStringList>

GpuMonitor::GpuMonitor(QObject *parent)
    : QObject(parent)
{
    connect(&m_timer, &QTimer::timeout,
            this, &GpuMonitor::refresh);

    refresh();
    m_timer.start(2000);
}

double GpuMonitor::usage() const
{
    return m_usage;
}

double GpuMonitor::temperature() const
{
    return m_temperature;
}

double GpuMonitor::usedVRAM() const
{
    return m_usedVRAM;
}

double GpuMonitor::totalVRAM() const
{
    return m_totalVRAM;
}

QString GpuMonitor::name() const
{
    return m_name;
}

bool GpuMonitor::available() const
{
    return m_available;
}

void GpuMonitor::refresh()
{
    QProcess process;

    process.start("nvidia-smi", {
        "--query-gpu=name,utilization.gpu,temperature.gpu,"
        "memory.used,memory.total",
        "--format=csv,noheader,nounits"
    });

    if (!process.waitForStarted(1000)
        || !process.waitForFinished(2000)
        || process.exitStatus() != QProcess::NormalExit
        || process.exitCode() != 0) {
        m_available = false;
        m_name = "GPU unavailable";
        emit updated();
        return;
    }

    const QString output =
        QString::fromLocal8Bit(process.readAllStandardOutput()).trimmed();

    const QStringList fields = output.split(',');

    if (fields.size() < 5) {
        m_available = false;
        m_name = "GPU unavailable";
        emit updated();
        return;
    }

    bool okUsage = false;
    bool okTemperature = false;
    bool okUsed = false;
    bool okTotal = false;

    const double usageValue =
        fields[1].trimmed().toDouble(&okUsage);
    const double temperatureValue =
        fields[2].trimmed().toDouble(&okTemperature);
    const double usedValue =
        fields[3].trimmed().toDouble(&okUsed);
    const double totalValue =
        fields[4].trimmed().toDouble(&okTotal);

    if (!okUsage || !okTemperature || !okUsed
        || !okTotal || totalValue <= 0) {
        m_available = false;
        m_name = "GPU unavailable";
        emit updated();
        return;
    }

    m_name = fields[0].trimmed();
    m_usage = usageValue;
    m_temperature = temperatureValue;
    m_usedVRAM = usedValue;
    m_totalVRAM = totalValue;
    m_available = true;

    emit updated();
}
