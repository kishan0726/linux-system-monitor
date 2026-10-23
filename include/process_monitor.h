
#ifndef PROCESS_MONITOR_H
#define PROCESS_MONITOR_H

#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <QElapsedTimer>

#include <QHash>

class ProcessMonitor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantList processes READ processes NOTIFY updated)
    Q_PROPERTY(int processCount READ processCount NOTIFY updated)

public:
    explicit ProcessMonitor(QObject *parent = nullptr);

    QVariantList processes() const;
    int processCount() const;

    Q_INVOKABLE bool terminateProcess(int pid);

signals:
    void updated();
    void terminationResult(int pid, bool success, const QString &message);

private slots:
    void refresh();

private:
    QVariantList m_processes;
    int m_processCount = 0;

    QHash<int, qulonglong> m_previousTicks;
    QElapsedTimer m_elapsed;
    QTimer m_timer;
};

#endif
