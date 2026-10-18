
#include "process_monitor.h"

#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include <QTextStream>
#include <QVariantMap>

#include <algorithm>

#include <QHash>
#include <unistd.h>

namespace {

struct RawProcess {
    int pid = 0;
    QString name;
    qulonglong cpuTicks = 0;
    qulonglong memoryKB = 0;
};

bool readProcess(int pid, RawProcess &process)
{
    const QString base = "/proc/" + QString::number(pid);

    QFile statFile(base + "/stat");

    if (!statFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    const QString stat = QString::fromLocal8Bit(
        statFile.readAll()).trimmed();

    // The process name inside parentheses can contain spaces.
    const qsizetype openParen = stat.indexOf('(');
    const qsizetype closeParen = stat.lastIndexOf(')');

    if (openParen < 0 || closeParen <= openParen)
        return false;

    process.pid = pid;
    process.name = stat.mid(openParen + 1,
                            closeParen - openParen - 1);

    // After ") ", the first field is field 3 (state).
    const QStringList fields = stat.mid(closeParen + 2)
        .split(QRegularExpression("\\s+"),
               Qt::SkipEmptyParts);

    // Fields 14 and 15 are utime and stime.
    // Relative to the substring above, these are indices 11 and 12.
    if (fields.size() <= 12)
        return false;

    bool userOk = false;
    bool systemOk = false;

    const qulonglong userTicks =
        fields[11].toULongLong(&userOk);
    const qulonglong systemTicks =
        fields[12].toULongLong(&systemOk);

    if (!userOk || !systemOk)
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

                if (values.size() >= 2)
                    process.memoryKB =
                        values[1].toULongLong();

                break;
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
    static QHash<int, qulonglong> previousTicks;

    QHash<int, qulonglong> currentTicks;
    QList<QVariantMap> rows;

    const long ticksPerSecond = sysconf(_SC_CLK_TCK);
    const double elapsedSeconds = 2.0;

    QDir procDir("/proc");

    const QStringList entries = procDir.entryList(
        QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString &entry : entries) {
        bool pidOk = false;
        const int pid = entry.toInt(&pidOk);

        if (!pidOk)
            continue;

        RawProcess process;

        if (!readProcess(pid, process))
            continue;

        currentTicks.insert(pid, process.cpuTicks);

        double cpuPercent = 0.0;

        if (previousTicks.contains(pid)
            && process.cpuTicks >= previousTicks.value(pid)
            && ticksPerSecond > 0) {
            cpuPercent =
                static_cast<double>(
                    process.cpuTicks - previousTicks.value(pid))
                / (ticksPerSecond * elapsedSeconds) * 100.0;
        }

        QVariantMap row;
        row.insert("pid", process.pid);
        row.insert("name", process.name);
        row.insert("cpu", cpuPercent);
        row.insert("memoryMB",
                   process.memoryKB / 1024.0);

        rows.append(row);
    }

    previousTicks = currentTicks;
    m_processCount = currentTicks.size();

    std::sort(rows.begin(), rows.end(),
              [](const QVariantMap &a, const QVariantMap &b) {
        return a.value("cpu").toDouble()
             > b.value("cpu").toDouble();
    });

    m_processes.clear();

    const int count = static_cast<int>(std::min(qsizetype{5}, rows.size()));

    for (int i = 0; i < count; ++i)
        m_processes.append(rows[i]);

    emit updated();
}
