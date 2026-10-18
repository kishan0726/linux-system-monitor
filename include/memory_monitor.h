
#ifndef MEMORY_MONITOR_H
#define MEMORY_MONITOR_H

#include <QObject>
#include <QTimer>
#include <QVariantList>

class MemoryMonitor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double usage READ usage NOTIFY updated)
    Q_PROPERTY(double totalGB READ totalGB NOTIFY updated)
    Q_PROPERTY(double usedGB READ usedGB NOTIFY updated)
    Q_PROPERTY(double availableGB READ availableGB NOTIFY updated)
    Q_PROPERTY(QVariantList history READ history NOTIFY updated)

public:
    explicit MemoryMonitor(QObject *parent = nullptr);

    double usage() const;
    double totalGB() const;
    double usedGB() const;
    double availableGB() const;
    QVariantList history() const;

signals:
    void updated();

private slots:
    void refresh();

private:
    double m_usage = 0.0;
    double m_totalGB = 0.0;
    double m_usedGB = 0.0;
    double m_availableGB = 0.0;

    QVariantList m_history;
    QTimer m_timer;
};

#endif
