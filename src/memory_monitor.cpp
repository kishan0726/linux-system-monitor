#include "memory_monitor.h"

#include <fstream>
#include <sstream>
#include <string>

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

double MemoryMonitor::freeGB() const
{
    return m_freeGB;
}

double MemoryMonitor::cachedGB() const
{
    return m_cachedGB;
}

double MemoryMonitor::buffersGB() const
{
    return m_buffersGB;
}

QVariantList MemoryMonitor::history() const
{
    return m_history;
}

double MemoryMonitor::swapTotalGB() const
{
    return m_swapTotalGB;
}

double MemoryMonitor::swapUsedGB() const
{
    return m_swapUsedGB;
}

double MemoryMonitor::swapFreeGB() const
{
    return m_swapFreeGB;
}

double MemoryMonitor::swapUsage() const
{
    return m_swapUsage;
}

QVariantList MemoryMonitor::swapHistory() const
{
    return m_swapHistory;
}

void MemoryMonitor::refresh()
{
    std::ifstream file("/proc/meminfo");

    if (!file.is_open())
        return;

    unsigned long long totalKB = 0;
    unsigned long long availableKB = 0;
    unsigned long long freeKB = 0;
    unsigned long long cachedKB = 0;
    unsigned long long buffersKB = 0;
    unsigned long long swapTotalKB = 0;
    unsigned long long swapFreeKB = 0;

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
        else if (key == "MemFree:")
            freeKB = value;
        else if (key == "Cached:")
            cachedKB = value;
        else if (key == "Buffers:")
            buffersKB = value;
        else if (key == "SwapTotal:")
            swapTotalKB = value;
        else if (key == "SwapFree:")
            swapFreeKB = value;
    }

    if (totalKB == 0)
        return;

    constexpr double KIB_PER_GIB = 1024.0 * 1024.0;

    // RAM values
    const unsigned long long usedKB =
        totalKB > availableKB ? totalKB - availableKB : 0;

    m_totalGB = totalKB / KIB_PER_GIB;
    m_usedGB = usedKB / KIB_PER_GIB;
    m_availableGB = availableKB / KIB_PER_GIB;
    m_freeGB = freeKB / KIB_PER_GIB;
    m_cachedGB = cachedKB / KIB_PER_GIB;
    m_buffersGB = buffersKB / KIB_PER_GIB;

    m_usage = 100.0 * static_cast<double>(usedKB)
                    / static_cast<double>(totalKB);

    // Swap values
    const unsigned long long swapUsedKB =
        swapTotalKB > swapFreeKB
            ? swapTotalKB - swapFreeKB
            : 0;

    m_swapTotalGB = swapTotalKB / KIB_PER_GIB;
    m_swapFreeGB = swapFreeKB / KIB_PER_GIB;
    m_swapUsedGB = swapUsedKB / KIB_PER_GIB;

    m_swapUsage = swapTotalKB > 0
        ? 100.0 * static_cast<double>(swapUsedKB)
                    / static_cast<double>(swapTotalKB)
        : 0.0;

    // Keep the latest 120 samples for each chart.
    m_history.append(m_usage);
    m_swapHistory.append(m_swapUsage);

    constexpr qsizetype MAX_SAMPLES = 120;

    while (m_history.size() > MAX_SAMPLES)
        m_history.removeFirst();

    while (m_swapHistory.size() > MAX_SAMPLES)
        m_swapHistory.removeFirst();

    emit updated();
}