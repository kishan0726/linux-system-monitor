
#include "cpu_monitor.h"

#include <QDir>
#include <QFile>
#include <QDebug>
#include <QVariantMap>
#include <QStringList>

#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

qulonglong CpuMonitor::RawCpuStats::total() const
{
    return user + nice + system + idle +
           iowait + irq + softirq + steal;
}

qulonglong CpuMonitor::RawCpuStats::idleTotal() const
{
    return idle + iowait;
}

CpuMonitor::CpuMonitor(QObject *parent)
    : QObject(parent)
{
    m_previousStats = readCPUStats();

    m_coreCount = static_cast<int>(
        std::count_if(
            m_previousStats.begin(),
            m_previousStats.end(),
            [](const RawCpuStats &cpu) {
                return cpu.name != "cpu";
            }
        )
    );

    m_temperature = getCPUTemperature();
    m_frequency = getCPUFrequency(0);

    connect(&m_timer, &QTimer::timeout,
            this, &CpuMonitor::refresh);

    m_timer.start(500);

    // Establish the baseline before calculating utilization.
    // The first valid percentage will appear on the next sample.
    refresh();
}

double CpuMonitor::usage() const
{
    return m_usage;
}

double CpuMonitor::temperature() const
{
    return m_temperature;
}

double CpuMonitor::frequency() const
{
    return m_frequency;
}

int CpuMonitor::coreCount() const
{
    return m_coreCount;
}

QVariantList CpuMonitor::cores() const
{
    return m_cores;
}

QVariantList CpuMonitor::history() const
{
    return m_history;
}

QVariantList CpuMonitor::coreHistory() const
{
    return m_coreHistory;
}

std::vector<CpuMonitor::RawCpuStats>
CpuMonitor::readCPUStats() const
{
    std::ifstream file("/proc/stat");
    std::vector<RawCpuStats> stats;

    if (!file.is_open()) {
        qWarning() << "Failed to open /proc/stat";
        return stats;
    }

    std::string line;

    while (std::getline(file, line)) {
        std::istringstream stream(line);
        std::string name;

        stream >> name;

        if (name != "cpu" &&
            name.rfind("cpu", 0) != 0) {
            break;
        }

        if (name != "cpu" &&
            (name.size() <= 3 ||
             !std::all_of(
                 name.begin() + 3,
                 name.end(),
                 [](unsigned char c) {
                     return c >= '0' && c <= '9';
                 }))) {
            break;
        }

        RawCpuStats cpu;
        cpu.name = QString::fromStdString(name);

        stream >> cpu.user
               >> cpu.nice
               >> cpu.system
               >> cpu.idle
               >> cpu.iowait
               >> cpu.irq
               >> cpu.softirq
               >> cpu.steal;

        if (stream.fail()) {
            qWarning() << "Failed to parse CPU row:"
                       << QString::fromStdString(line);
            continue;
        }

        stats.push_back(cpu);
    }

    return stats;
}

double CpuMonitor::calculateCPUUsage(
    const RawCpuStats &first,
    const RawCpuStats &second) const
{
    const qulonglong firstTotal = first.total();
    const qulonglong secondTotal = second.total();

    if (secondTotal <= firstTotal)
        return 0.0;

    const qulonglong totalDiff = secondTotal - firstTotal;
    const qulonglong firstIdle = first.idleTotal();
    const qulonglong secondIdle = second.idleTotal();

    if (secondIdle < firstIdle)
        return 0.0;

    const qulonglong idleDiff = secondIdle - firstIdle;

    if (idleDiff > totalDiff)
        return 0.0;

    return 100.0 *
           static_cast<double>(totalDiff - idleDiff) /
           static_cast<double>(totalDiff);
}

double CpuMonitor::getCPUFrequency(int cpuNumber) const
{
    const QString path =
        QString("/sys/devices/system/cpu/cpu%1/cpufreq/"
                "scaling_cur_freq").arg(cpuNumber);

    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return 0.0;

    bool ok = false;
    const double khz =
        QString::fromLatin1(file.readAll()).trimmed().toDouble(&ok);

    return ok ? khz / 1000.0 : 0.0;
}

double CpuMonitor::getCPUTemperature() const
{
    QDir hwmon("/sys/class/hwmon");

    if (!hwmon.exists())
        return 0.0;

    const QStringList entries =
        hwmon.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString &entry : entries) {
        const QString base = hwmon.filePath(entry);

        QFile nameFile(base + "/name");

        if (!nameFile.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;

        const QString name =
            QString::fromLatin1(nameFile.readAll()).trimmed();

        if (name != "coretemp")
            continue;

        QDir sensorDir(base);

        const QStringList labels =
            sensorDir.entryList(
                QStringList{"temp*_label"}, QDir::Files);

        for (const QString &labelFileName : labels) {
            QFile labelFile(base + "/" + labelFileName);

            if (!labelFile.open(QIODevice::ReadOnly | QIODevice::Text))
                continue;

            const QString label =
                QString::fromLatin1(labelFile.readAll()).trimmed();

            if (label != "Package id 0")
                continue;

            QString inputName = labelFileName;
            inputName.chop(6);
            inputName += "_input";

            QFile inputFile(base + "/" + inputName);

            if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
                return 0.0;

            bool ok = false;
            const double milliDegrees =
                QString::fromLatin1(inputFile.readAll())
                    .trimmed().toDouble(&ok);

            return ok ? milliDegrees / 1000.0 : 0.0;
        }
    }

    return 0.0;
}

void CpuMonitor::refresh()
{
    const auto current = readCPUStats();

    if (current.empty())
        return;

    QVariantList coreValues;
    QVariantList coreHistoryValues;

    for (size_t i = 0; i < current.size(); ++i) {
        double percent = 0.0;

        if (i < m_previousStats.size() &&
            current[i].name == m_previousStats[i].name) {
            percent = calculateCPUUsage(
                m_previousStats[i], current[i]);
        }

        percent = std::clamp(percent, 0.0, 100.0);

        if (current[i].name == "cpu") {
            m_usage = percent;
        } else {
            QVariantMap core;
            core["name"] = current[i].name;
            core["usage"] = percent;
            coreValues.append(core);

            QVariantList samples = m_perCoreHistory.value(current[i].name);
            samples.append(percent);
            while (samples.size() > 120)
                samples.removeFirst();
            m_perCoreHistory.insert(current[i].name, samples);

            QVariantMap historyEntry;
            historyEntry["name"] = current[i].name;
            historyEntry["usage"] = percent;
            historyEntry["history"] = samples;
            coreHistoryValues.append(historyEntry);
        }
    }

    m_cores = coreValues;
    m_coreHistory = coreHistoryValues;
    m_coreCount = coreValues.size();

    m_previousStats = current;

    m_frequency = getCPUFrequency(0);
    m_temperature = getCPUTemperature();

    // Keep the latest 60 real CPU usage samples.
    m_history.append(m_usage);

    while (m_history.size() > 60)
        m_history.removeFirst();

    emit updated();
}
