#ifndef MEMORY_MONITOR_H
#define MEMORY_MONITOR_H

#include <QObject>
#include <QTimer>
#include <QVariantList>

class MemoryMonitor : public QObject
{
    Q_OBJECT

    // RAM information
    Q_PROPERTY(double usage READ usage NOTIFY updated)
    Q_PROPERTY(double totalGB READ totalGB NOTIFY updated)
    Q_PROPERTY(double usedGB READ usedGB NOTIFY updated)
    Q_PROPERTY(double availableGB READ availableGB NOTIFY updated)
    Q_PROPERTY(double freeGB READ freeGB NOTIFY updated)
    Q_PROPERTY(double cachedGB READ cachedGB NOTIFY updated)
    Q_PROPERTY(double buffersGB READ buffersGB NOTIFY updated)
    Q_PROPERTY(QVariantList history READ history NOTIFY updated)

    // Swap information
    Q_PROPERTY(double swapTotalGB READ swapTotalGB NOTIFY updated)
    Q_PROPERTY(double swapUsedGB READ swapUsedGB NOTIFY updated)
    Q_PROPERTY(double swapFreeGB READ swapFreeGB NOTIFY updated)
    Q_PROPERTY(double swapUsage READ swapUsage NOTIFY updated)
    Q_PROPERTY(QVariantList swapHistory READ swapHistory NOTIFY updated)

public:
    explicit MemoryMonitor(QObject *parent = nullptr);

    double usage() const;
    double totalGB() const;
    double usedGB() const;
    double availableGB() const;
    double freeGB() const;
    double cachedGB() const;
    double buffersGB() const;
    QVariantList history() const;

    double swapTotalGB() const;
    double swapUsedGB() const;
    double swapFreeGB() const;
    double swapUsage() const;
    QVariantList swapHistory() const;

signals:
    void updated();

private slots:
    void refresh();

private:
    double m_usage = 0.0;

    double m_totalGB = 0.0;
    double m_usedGB = 0.0;
    double m_availableGB = 0.0;
    double m_freeGB = 0.0;
    double m_cachedGB = 0.0;
    double m_buffersGB = 0.0;

    double m_swapTotalGB = 0.0;
    double m_swapUsedGB = 0.0;
    double m_swapFreeGB = 0.0;
    double m_swapUsage = 0.0;

    QVariantList m_history;
    QVariantList m_swapHistory;

    QTimer m_timer;
};

#endif