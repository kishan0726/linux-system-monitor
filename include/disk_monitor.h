
#ifndef DISK_MONITOR_H
#define DISK_MONITOR_H

#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <QtGlobal>

class DiskMonitor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double usage READ usage NOTIFY updated)
    Q_PROPERTY(double totalGB READ totalGB NOTIFY updated)
    Q_PROPERTY(double usedGB READ usedGB NOTIFY updated)
    Q_PROPERTY(double freeGB READ freeGB NOTIFY updated)

    Q_PROPERTY(double readMBps READ readMBps NOTIFY updated)
    Q_PROPERTY(double writeMBps READ writeMBps NOTIFY updated)

    Q_PROPERTY(QVariantList history READ history NOTIFY updated)
    Q_PROPERTY(QVariantList readHistory READ readHistory NOTIFY updated)
    Q_PROPERTY(QVariantList writeHistory READ writeHistory NOTIFY updated)

public:
    explicit DiskMonitor(QObject *parent = nullptr);

    double usage() const;
    double totalGB() const;
    double usedGB() const;
    double freeGB() const;

    double readMBps() const;
    double writeMBps() const;

    QVariantList history() const;
    QVariantList readHistory() const;
    QVariantList writeHistory() const;

signals:
    void updated();

private slots:
    void refresh();

private:
    double m_usage = 0.0;
    double m_totalGB = 0.0;
    double m_usedGB = 0.0;
    double m_freeGB = 0.0;

    double m_readMBps = 0.0;
    double m_writeMBps = 0.0;

    quint64 m_previousReadSectors = 0;
    quint64 m_previousWriteSectors = 0;
    bool m_firstSample = true;

    QVariantList m_history;
    QVariantList m_readHistory;
    QVariantList m_writeHistory;

    QTimer m_timer;
};

#endif
