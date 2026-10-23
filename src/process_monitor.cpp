
#include "process_monitor.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QTextStream>
#include <QVariantMap>
#include <QCoreApplication>
#include <QHash>
#include <QSet>
#include <QProcess>
#include <QDebug>

#include <algorithm>
#include <unistd.h>
#include <signal.h>
#include <cerrno>

namespace {

struct RawProcess {
    int pid = 0;
    QString name;
    QString state;
    qulonglong cpuTicks = 0;
    qulonglong memoryKB = 0;
    uint userId = 0;
};

bool readProcess(int pid, RawProcess &process)
{
    const QString base = "/proc/" + QString::number(pid);

    QFile statFile(base + "/stat");

    if (!statFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    const QString stat =
        QString::fromLocal8Bit(statFile.readAll()).trimmed();

    // The process name can contain spaces and parentheses.
    const qsizetype openParen = stat.indexOf('(');
    const qsizetype closeParen = stat.lastIndexOf(')');

    if (openParen < 0 || closeParen <= openParen)
        return false;

    process.pid = pid;
    process.name = stat.mid(openParen + 1,
                            closeParen - openParen - 1);

    // After ") ", index 0 is field 3 (state).
    const QStringList fields = stat.mid(closeParen + 2)
        .split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    // Relative indices: state=0, utime=11, stime=12.
    if (fields.size() <= 12)
        return false;

    process.state = fields[0];

    bool userTicksOK = false;
    bool systemTicksOK = false;

    const qulonglong userTicks =
        fields[11].toULongLong(&userTicksOK);
    const qulonglong systemTicks =
        fields[12].toULongLong(&systemTicksOK);

    if (!userTicksOK || !systemTicksOK)
        return false;

    process.cpuTicks = userTicks + systemTicks;

    QFile statusFile(base + "/status");

    if (statusFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&statusFile);

        while (!stream.atEnd()) {
            const QString line = stream.readLine();

            if (line.startsWith("VmRSS:")) {
                const QStringList values = line.split(
                    QRegularExpression("\\s+"),
                    Qt::SkipEmptyParts);

                if (values.size() >= 2) {
                    bool ok = false;
                    const qulonglong memory =
                        values[1].toULongLong(&ok);

                    if (ok)
                        process.memoryKB = memory;
                }
            } else if (line.startsWith("Uid:")) {
                const QStringList values = line.split(
                    QRegularExpression("\\s+"),
                    Qt::SkipEmptyParts);

                if (values.size() >= 2) {
                    bool ok = false;
                    const uint uid = values[1].toUInt(&ok);

                    if (ok)
                        process.userId = uid;
                }
            }
        }
    }

    return true;
}

} // namespace

ProcessMonitor::ProcessMonitor(QObject *parent)
    : QObject(parent)
{
    connect(&m_timer, &QTimer::timeout,
            this, &ProcessMonitor::refresh);

    m_elapsed.start();
    refresh();
    m_timer.start(2000);
}

QVariantList ProcessMonitor::processes() const
{
    return m_processes;
}

int ProcessMonitor::processCount() const
{
    return m_processCount;
}

void ProcessMonitor::refresh()
{
    const qint64 elapsedMs = m_elapsed.isValid()
        ? m_elapsed.restart()
        : 2000;

    const double elapsedSeconds =
        elapsedMs > 0 ? elapsedMs / 1000.0 : 2.0;

    const long ticksPerSecond = sysconf(_SC_CLK_TCK);

    QHash<int, qulonglong> currentTicks;
    QVariantList rows;

    QDir procDir("/proc");
    const QStringList entries = procDir.entryList(
        QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString &entry : entries) {
        bool pidOK = false;
        const int pid = entry.toInt(&pidOK);

        if (!pidOK || pid <= 0)
            continue;

        RawProcess process;

        if (!readProcess(pid, process))
            continue;

        currentTicks.insert(pid, process.cpuTicks);

        double cpuPercent = 0.0;

        if (m_previousTicks.contains(pid)
            && process.cpuTicks >= m_previousTicks.value(pid)
            && ticksPerSecond > 0) {
            cpuPercent =
                static_cast<double>(
                    process.cpuTicks - m_previousTicks.value(pid))
                / (ticksPerSecond * elapsedSeconds) * 100.0;
        }

        QVariantMap row;
        row.insert("pid", process.pid);
        row.insert("name", process.name);
        row.insert("state", process.state);
        row.insert("cpu", cpuPercent);
        row.insert("memoryMB", process.memoryKB / 1024.0);
        row.insert("memoryKB",
                   QVariant::fromValue(process.memoryKB));
        row.insert("uid", process.userId);

        rows.append(row);
    }

    m_previousTicks = currentTicks;
    m_processCount = rows.size();

    m_processes = rows;

    emit updated();
}

bool ProcessMonitor::terminateProcess(int pid)
{
    // Never allow this helper to target PID 1 or this application.
    if (pid <= 1 || pid == QCoreApplication::applicationPid()) {
        emit terminationResult(
            pid, false, "Protected process: termination was blocked.");
        return false;
    }

    // Verify the process still exists and can be inspected.
    RawProcess process;

    if (!readProcess(pid, process)) {
        emit terminationResult(
            pid, false, "Process no longer exists or cannot be inspected.");
        refresh();
        return false;
    }

    // Send SIGTERM first, allowing the process to exit gracefully.
    if (::kill(pid, SIGTERM) == 0) {
        emit terminationResult(
            pid, true, "SIGTERM sent. The process may take time to exit.");
        return true;
    }

    const int error = errno;
    QString message;

    if (error == EPERM)
        message = "Permission denied. You cannot terminate this process.";
    else if (error == ESRCH)
        message = "Process no longer exists.";
    else
        message = "Failed to terminate process (errno "
            + QString::number(error) + ").";

    emit terminationResult(pid, false, message);
    return false;
}
