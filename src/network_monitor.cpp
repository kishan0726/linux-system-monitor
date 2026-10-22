
#include "network_monitor.h"

#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QHash>

namespace {
constexpr qsizetype MAX_HISTORY = 120;
constexpr double BYTES_PER_MIB = 1024.0 * 1024.0;

struct InterfaceStats {
    qulonglong received = 0;
    qulonglong sent = 0;

    qulonglong total() const
    {
        return received + sent;
    }
};
}

NetworkMonitor::NetworkMonitor(QObject *parent)
    : QObject(parent)
{
    connect(&m_timer, &QTimer::timeout,
            this, &NetworkMonitor::refresh);

    refresh();
    m_elapsed.start();
    m_timer.start(1000);
}

bool NetworkMonitor::available() const
{
    return m_available;
}

QString NetworkMonitor::interfaceName() const
{
    return m_interfaceName;
}

double NetworkMonitor::downloadMBps() const
{
    return m_downloadMBps;
}

double NetworkMonitor::uploadMBps() const
{
    return m_uploadMBps;
}

qulonglong NetworkMonitor::receivedBytes() const
{
    return m_receivedBytes;
}

qulonglong NetworkMonitor::sentBytes() const
{
    return m_sentBytes;
}

QVariantList NetworkMonitor::downloadHistory() const
{
    return m_downloadHistory;
}

QVariantList NetworkMonitor::uploadHistory() const
{
    return m_uploadHistory;
}

QString NetworkMonitor::defaultInterface() const
{
    QFile file("/proc/net/route");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    QTextStream in(&file);
    in.readLine(); // Skip header

    while (!in.atEnd()) {
        const QStringList fields =
            in.readLine().trimmed().split(
                QLatin1Char(' '), Qt::SkipEmptyParts);

        if (fields.size() < 4)
            continue;

        const QString interface = fields[0];
        const QString destination = fields[1];

        bool flagsOK = false;
        const uint flags = fields[3].toUInt(&flagsOK, 16);

        if (destination == "00000000"
            && flagsOK
            && (flags & 0x1)
            && interface != "lo") {
            return interface;
        }
    }

    return {};
}

void NetworkMonitor::refresh()
{
    const QString routeInterface = defaultInterface();

    QFile file("/proc/net/dev");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_available = false;
        m_interfaceName = "Unavailable";
        m_downloadMBps = 0.0;
        m_uploadMBps = 0.0;
        emit updated();
        return;
    }

    QHash<QString, InterfaceStats> interfaces;

    QTextStream in(&file);
    in.readLine();
    in.readLine();

    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        const qsizetype colon = line.indexOf(':');

        if (colon < 0)
            continue;

        const QString name = line.left(colon).trimmed();

        if (name.isEmpty() || name == "lo")
            continue;

        const QStringList values =
            line.mid(colon + 1).split(
                QLatin1Char(' '), Qt::SkipEmptyParts);

        // RX bytes is field 0; TX bytes is field 8.
        if (values.size() < 9)
            continue;

        bool rxOK = false;
        bool txOK = false;

        const qulonglong rx = values[0].toULongLong(&rxOK);
        const qulonglong tx = values[8].toULongLong(&txOK);

        if (!rxOK || !txOK)
            continue;

        interfaces.insert(name, {rx, tx});
    }

    QString selectedInterface;

    // Prefer the default route if it has accumulated traffic.
    if (interfaces.contains(routeInterface)) {
        const InterfaceStats routeStats = interfaces.value(routeInterface);

        if (routeStats.total() > 0)
            selectedInterface = routeInterface;
    }

    // Otherwise choose the interface with the most accumulated traffic.
    if (selectedInterface.isEmpty()) {
        qulonglong highestTraffic = 0;

        for (auto it = interfaces.cbegin();
             it != interfaces.cend(); ++it) {
            if (it.value().total() > highestTraffic) {
                highestTraffic = it.value().total();
                selectedInterface = it.key();
            }
        }
    }

    const bool wasAvailable = m_available;
    const QString previousInterface = m_interfaceName;

    if (selectedInterface.isEmpty()) {
        m_available = false;
        m_interfaceName = "Unavailable";
        m_downloadMBps = 0.0;
        m_uploadMBps = 0.0;
        m_firstSample = true;
        m_previousReceivedBytes = 0;
        m_previousSentBytes = 0;
    } else {
        const InterfaceStats stats = interfaces.value(selectedInterface);

        m_available = true;
        m_interfaceName = selectedInterface;
        m_receivedBytes = stats.received;
        m_sentBytes = stats.sent;

        const qint64 elapsedMs = m_elapsed.isValid()
            ? m_elapsed.restart()
            : 0;

        const double seconds = elapsedMs > 0
            ? elapsedMs / 1000.0
            : 1.0;

        const bool interfaceChanged =
            !wasAvailable || previousInterface != selectedInterface;

        if (m_firstSample || interfaceChanged) {
            m_downloadMBps = 0.0;
            m_uploadMBps = 0.0;
            m_firstSample = false;
        } else {
            m_downloadMBps =
                stats.received >= m_previousReceivedBytes
                ? (stats.received - m_previousReceivedBytes)
                    / BYTES_PER_MIB / seconds
                : 0.0;

            m_uploadMBps =
                stats.sent >= m_previousSentBytes
                ? (stats.sent - m_previousSentBytes)
                    / BYTES_PER_MIB / seconds
                : 0.0;
        }

        m_previousReceivedBytes = stats.received;
        m_previousSentBytes = stats.sent;
    }

    m_downloadHistory.append(m_downloadMBps);
    m_uploadHistory.append(m_uploadMBps);

    while (m_downloadHistory.size() > MAX_HISTORY)
        m_downloadHistory.removeFirst();

    while (m_uploadHistory.size() > MAX_HISTORY)
        m_uploadHistory.removeFirst();

    emit updated();
}
