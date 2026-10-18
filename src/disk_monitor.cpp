
#include "disk_monitor.h"

#include <filesystem>

namespace fs = std::filesystem;

DiskMonitor::DiskMonitor(QObject *parent)
    : QObject(parent)
{
    connect(&m_timer, &QTimer::timeout,
            this, &DiskMonitor::refresh);

    refresh();
    m_timer.start(2000);
}

double DiskMonitor::usage() const
{
    return m_usage;
}

double DiskMonitor::totalGB() const
{
    return m_totalGB;
}

double DiskMonitor::usedGB() const
{
    return m_usedGB;
}

double DiskMonitor::freeGB() const
{
    return m_freeGB;
}

void DiskMonitor::refresh()
{
    try {
        const auto info = fs::space("/");

        if (info.capacity == 0)
            return;

        constexpr double BYTES_PER_GIB =
            1024.0 * 1024.0 * 1024.0;

        m_totalGB =
            static_cast<double>(info.capacity) / BYTES_PER_GIB;

        m_freeGB =
            static_cast<double>(info.available) / BYTES_PER_GIB;

        m_usedGB = m_totalGB - m_freeGB;

        m_usage =
            100.0 * static_cast<double>(
                info.capacity - info.available)
                / static_cast<double>(info.capacity);

        emit updated();
    }
    catch (const fs::filesystem_error &) {
        // Keep the previous values if the filesystem query fails.
    }
}
