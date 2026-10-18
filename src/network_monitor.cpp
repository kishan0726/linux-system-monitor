
#include "network_monitor.h"

#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

NetworkMonitor::NetworkMonitor(QObject *parent)
    : QObject(parent)
{
    connect(&m_timer, &QTimer::timeout,
            this, &NetworkMonitor::refresh);

    refresh();
    m_timer.start(1000);
}

double NetworkMonitor::downloadMBps() const
{
    return m_downloadMBps;
}

double NetworkMonitor::uploadMBps() const
{
    return m_uploadMBps;
}

QString NetworkMonitor::interfaceName() const
{
    return m_interfaceName;
}

bool NetworkMonitor::available() const
{
    return m_available;
}

void NetworkMonitor::refresh()
{
    // Find the interface used by the default IPv4 route.
    QString defaultInterface;

    QFile routeFile("/proc/net/route");

    if (routeFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream routeStream(&routeFile);
        routeStream.readLine(); // Header

        while (!routeStream.atEnd()) {
            const QString line = routeStream.readLine().trimmed();
            const QStringList fields = line.split(
                QRegularExpression("\\s+"));

            if (fields.size() > 7
                && fields[1] == "00000000"
                && (fields[3].toUInt(nullptr, 16) & 1)) {
                defaultInterface = fields[0];
                break;
            }
        }
    }

    if (defaultInterface.isEmpty()) {
        m_available = false;
        m_hasPreviousSample = false;
        m_interfaceName = "N/A";
        m_downloadMBps = 0.0;
        m_uploadMBps = 0.0;
        emit updated();
        return;
    }

    QFile netFile("/proc/net/dev");

    if (!netFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_available = false;
        m_hasPreviousSample = false;
        emit updated();
        return;
    }

    QTextStream netStream(&netFile);

    // Skip the two header lines.
    netStream.readLine();
    netStream.readLine();

    qulonglong rxBytes = 0;
    qulonglong txBytes = 0;
    bool found = false;

    while (!netStream.atEnd()) {
        const QString line = netStream.readLine().trimmed();
        const int colon = line.indexOf(':');

        if (colon < 0)
            continue;

        const QString iface = line.left(colon).trimmed();

        if (iface != defaultInterface)
            continue;

        const QStringList values = line.mid(colon + 1)
            .split(QRegularExpression("\\s+"),
                   Qt::SkipEmptyParts);

        if (values.size() < 9)
            break;

        bool rxOk = false;
        bool txOk = false;

        rxBytes = values[0].toULongLong(&rxOk);
        txBytes = values[8].toULongLong(&txOk);

        found = rxOk && txOk;
        break;
    }

    if (!found) {
        m_available = false;
        m_hasPreviousSample = false;
        m_interfaceName = defaultInterface;
        m_downloadMBps = 0.0;
        m_uploadMBps = 0.0;
        emit updated();
        return;
    }

    if (m_hasPreviousSample
        && rxBytes >= m_previousRxBytes
        && txBytes >= m_previousTxBytes) {
        // Convert bytes per second to decimal MB/s.
        constexpr double BYTES_PER_MB = 1000000.0;

        m_downloadMBps =
            static_cast<double>(rxBytes - m_previousRxBytes)
            / BYTES_PER_MB;

        m_uploadMBps =
            static_cast<double>(txBytes - m_previousTxBytes)
            / BYTES_PER_MB;
    } else {
        // First sample establishes the baseline.
        m_downloadMBps = 0.0;
        m_uploadMBps = 0.0;
    }

    m_previousRxBytes = rxBytes;
    m_previousTxBytes = txBytes;
    m_hasPreviousSample = true;

    m_interfaceName = defaultInterface;
    m_available = true;

    emit updated();
}
