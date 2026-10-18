
#ifndef PROCESS_MONITOR_H
#define PROCESS_MONITOR_H

#include <QObject>
#include <QTimer>
#include <QVariantList>

class ProcessMonitor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantList processes READ processes NOTIFY updated)
    Q_PROPERTY(int processCount READ processCount NOTIFY updated)

public:
    explicit ProcessMonitor(QObject *parent = nullptr);

    QVariantList processes() const;
    int processCount() const;

signals:
    void updated();

private slots:
    void refresh();

private:
    QVariantList m_processes;
    int m_processCount = 0;
    QTimer m_timer;
};

#endif
