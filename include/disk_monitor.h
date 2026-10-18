
#ifndef DISK_MONITOR_H
#define DISK_MONITOR_H

#include <QObject>
#include <QTimer>

class DiskMonitor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double usage READ usage NOTIFY updated)
    Q_PROPERTY(double totalGB READ totalGB NOTIFY updated)
    Q_PROPERTY(double usedGB READ usedGB NOTIFY updated)
    Q_PROPERTY(double freeGB READ freeGB NOTIFY updated)

public:
    explicit DiskMonitor(QObject *parent = nullptr);

    double usage() const;
    double totalGB() const;
    double usedGB() const;
    double freeGB() const;

signals:
    void updated();

private slots:
    void refresh();

private:
    double m_usage = 0.0;
    double m_totalGB = 0.0;
    double m_usedGB = 0.0;
    double m_freeGB = 0.0;

    QTimer m_timer;
};

#endif
