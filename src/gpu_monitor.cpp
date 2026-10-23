
#include "gpu_monitor.h"

#include <QProcess>
#include <QStringList>
#include <QVariant>
#include <QtGlobal>

namespace {
constexpr qsizetype MAX_HISTORY = 120;
constexpr int REFRESH_INTERVAL_MS = 2000;
}

GpuMonitor::GpuMonitor(QObject *parent)
    : QObject(parent)
{
    connect(&m_timer, &QTimer::timeout,
            this, &GpuMonitor::refresh);

    refresh();
    m_timer.start(REFRESH_INTERVAL_MS);
}

double GpuMonitor::usage() const { return m_usage; }
double GpuMonitor::temperature() const { return m_temperature; }
double GpuMonitor::usedVRAM() const { return m_usedVRAM; }
double GpuMonitor::totalVRAM() const { return m_totalVRAM; }
QString GpuMonitor::name() const { return m_name; }
bool GpuMonitor::available() const { return m_available; }

QVariantList GpuMonitor::history() const
{
    return m_history;
}

QVariantList GpuMonitor::vramHistory() const
{
    return m_vramHistory;
}

void GpuMonitor::refresh()
{
    QProcess process;

    process.start("nvidia-smi", {
        "--query-gpu=name,utilization.gpu,temperature.gpu,"
        "memory.used,memory.total",
        "--format=csv,noheader,nounits"
    });

    if (!process.waitForStarted(1000)) {
        m_available = false;
        m_name = "GPU unavailable";
        emit updated();
        return;
    }

    if (!process.waitForFinished(2000)) {
        process.terminate();

        if (!process.waitForFinished(300)) {
            process.kill();
            process.waitForFinished(1000);
        }

        m_available = false;
        m_name = "GPU unavailable";
        emit updated();
        return;
    }

    if (process.exitStatus() != QProcess::NormalExit
        || process.exitCode() != 0) {
        m_available = false;
        m_name = "GPU unavailable";
        emit updated();
        return;
    }

    const QString output =
        QString::fromLocal8Bit(process.readAllStandardOutput()).trimmed();

    if (output.isEmpty()) {
        m_available = false;
        m_name = "GPU unavailable";
        emit updated();
        return;
    }

    // Use the first GPU row if the system has multiple GPUs.
    const QString firstLine = output.section('\n', 0, 0).trimmed();
    const QStringList fields = firstLine.split(',');

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
        || !okTotal || totalValue <= 0
        || usageValue < 0 || usageValue > 100
        || usedValue < 0 || usedValue > totalValue) {
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

    // Keep GPU usage and VRAM history as numeric samples.
    m_history.append(m_usage);
    m_vramHistory.append(m_usedVRAM);

    while (m_history.size() > MAX_HISTORY)
        m_history.removeFirst();

    while (m_vramHistory.size() > MAX_HISTORY)
        m_vramHistory.removeFirst();

    emit updated();
}
