
#include "memory_monitor.h"

#include <fstream>
#include <string>
#include <sstream>

MemoryMonitor::MemoryMonitor(QObject *parent)
    : QObject(parent)
{
    connect(&m_timer, &QTimer::timeout,
            this, &MemoryMonitor::refresh);

    refresh();
    m_timer.start(500);
}

double MemoryMonitor::usage() const
{
    return m_usage;
}

double MemoryMonitor::totalGB() const
{
    return m_totalGB;
}

double MemoryMonitor::usedGB() const
{
    return m_usedGB;
}

double MemoryMonitor::availableGB() const
{
    return m_availableGB;
}

QVariantList MemoryMonitor::history() const
{
    return m_history;
}

void MemoryMonitor::refresh()
{
    std::ifstream file("/proc/meminfo");

    if (!file.is_open())
        return;

    unsigned long long totalKB = 0;
    unsigned long long availableKB = 0;

    std::string line;

    while (std::getline(file, line)) {
        std::istringstream stream(line);

        std::string key;
        unsigned long long value = 0;

        if (!(stream >> key >> value))
            continue;

        if (key == "MemTotal:")
            totalKB = value;
        else if (key == "MemAvailable:")
            availableKB = value;
    }

    if (totalKB == 0)
        return;

    const unsigned long long usedKB =
        totalKB - availableKB;

    // Linux reports these values in KiB.
    constexpr double KIB_PER_GIB = 1024.0 * 1024.0;

    m_totalGB = totalKB / KIB_PER_GIB;
    m_availableGB = availableKB / KIB_PER_GIB;
    m_usedGB = usedKB / KIB_PER_GIB;

    m_usage = 100.0 * static_cast<double>(usedKB)
                    / static_cast<double>(totalKB);

    m_history.append(m_usage);

    while (m_history.size() > 60)
        m_history.removeFirst();

    emit updated();
}
