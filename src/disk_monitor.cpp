
#include "disk_monitor.h"

#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QElapsedTimer>

#include <filesystem>

namespace fs = std::filesystem;

namespace {
constexpr double BYTES_PER_GIB = 1024.0 * 1024.0 * 1024.0;
constexpr double BYTES_PER_MIB = 1024.0 * 1024.0;
constexpr quint64 BYTES_PER_SECTOR = 512;
constexpr qsizetype MAX_HISTORY = 120;
}

DiskMonitor::DiskMonitor(QObject *parent)
    : QObject(parent)
{
    connect(&m_timer, &QTimer::timeout,
            this, &DiskMonitor::refresh);

    refresh();
    m_timer.start(2000);
}

double DiskMonitor::usage() const { return m_usage; }
double DiskMonitor::totalGB() const { return m_totalGB; }
double DiskMonitor::usedGB() const { return m_usedGB; }
double DiskMonitor::freeGB() const { return m_freeGB; }

double DiskMonitor::readMBps() const { return m_readMBps; }
double DiskMonitor::writeMBps() const { return m_writeMBps; }

QVariantList DiskMonitor::history() const { return m_history; }
QVariantList DiskMonitor::readHistory() const { return m_readHistory; }
QVariantList DiskMonitor::writeHistory() const { return m_writeHistory; }

void DiskMonitor::refresh()
{
    // 1. Filesystem capacity for the filesystem containing "/".
    try {
        const auto info = fs::space("/");

        if (info.capacity > 0) {
            m_totalGB =
                static_cast<double>(info.capacity) / BYTES_PER_GIB;

            m_freeGB =
                static_cast<double>(info.available) / BYTES_PER_GIB;

            m_usedGB = m_totalGB - m_freeGB;

            m_usage = 100.0 *
                static_cast<double>(info.capacity - info.available) /
                static_cast<double>(info.capacity);

            m_usage = qBound(0.0, m_usage, 100.0);

            m_history.append(m_usage);
        }
    } catch (const fs::filesystem_error &) {
        // Keep previous capacity values if the query fails.
    }

    // 2. Read aggregate I/O counters for whole disks.
    QFile file("/proc/diskstats");

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        quint64 readSectors = 0;
        quint64 writeSectors = 0;

        while (!file.atEnd()) {
            const QString line = QString::fromLocal8Bit(
                file.readLine()).trimmed();

            const QStringList fields =
                line.split(QChar::Space, Qt::SkipEmptyParts);

            // Need fields through sectors written (index 9).
            if (fields.size() < 10)
                continue;

            const QString device = fields[2];

            // /sys/block contains whole disks, not their partitions.
            if (!QFileInfo::exists("/sys/block/" + device))
                continue;

            bool readOK = false;
            bool writeOK = false;

            const quint64 currentRead =
                fields[5].toULongLong(&readOK);

            const quint64 currentWrite =
                fields[9].toULongLong(&writeOK);

            if (readOK)
                readSectors += currentRead;

            if (writeOK)
                writeSectors += currentWrite;
        }

        static QElapsedTimer elapsed;
        static bool elapsedStarted = false;

        if (!elapsedStarted) {
            elapsed.start();
            elapsedStarted = true;
        }

        const qint64 elapsedMs = elapsed.restart();
        const double seconds = elapsedMs > 0
            ? elapsedMs / 1000.0
            : 2.0;

        if (!m_firstSample) {
            if (readSectors >= m_previousReadSectors) {
                m_readMBps =
                    static_cast<double>(
                        (readSectors - m_previousReadSectors)
                        * BYTES_PER_SECTOR)
                    / BYTES_PER_MIB / seconds;
            } else {
                m_readMBps = 0.0;
            }

            if (writeSectors >= m_previousWriteSectors) {
                m_writeMBps =
                    static_cast<double>(
                        (writeSectors - m_previousWriteSectors)
                        * BYTES_PER_SECTOR)
                    / BYTES_PER_MIB / seconds;
            } else {
                m_writeMBps = 0.0;
            }
        } else {
            m_readMBps = 0.0;
            m_writeMBps = 0.0;
            m_firstSample = false;
        }

        m_previousReadSectors = readSectors;
        m_previousWriteSectors = writeSectors;
    }

    // 3. Keep the latest 120 samples for each chart.
    m_readHistory.append(m_readMBps);
    m_writeHistory.append(m_writeMBps);

    while (m_history.size() > MAX_HISTORY)
        m_history.removeFirst();

    while (m_readHistory.size() > MAX_HISTORY)
        m_readHistory.removeFirst();

    while (m_writeHistory.size() > MAX_HISTORY)
        m_writeHistory.removeFirst();

    emit updated();
}
