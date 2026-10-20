
#ifndef CPU_MONITOR_H
#define CPU_MONITOR_H

#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <QString>
#include <QHash>

#include <vector>

class CpuMonitor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double usage READ usage NOTIFY updated)
    Q_PROPERTY(double temperature READ temperature NOTIFY updated)
    Q_PROPERTY(double frequency READ frequency NOTIFY updated)
    Q_PROPERTY(int coreCount READ coreCount NOTIFY updated)
    Q_PROPERTY(QVariantList cores READ cores NOTIFY updated)
    Q_PROPERTY(QVariantList history READ history NOTIFY updated)
    Q_PROPERTY(QVariantList coreHistory READ coreHistory NOTIFY updated)

public:
    explicit CpuMonitor(QObject *parent = nullptr);

    double usage() const;
    double temperature() const;
    double frequency() const;
    int coreCount() const;

    QVariantList cores() const;
    QVariantList history() const;
    QVariantList coreHistory() const;

signals:
    void updated();

private slots:
    void refresh();

private:
    struct RawCpuStats {
        QString name;
        qulonglong user = 0;
        qulonglong nice = 0;
        qulonglong system = 0;
        qulonglong idle = 0;
        qulonglong iowait = 0;
        qulonglong irq = 0;
        qulonglong softirq = 0;
        qulonglong steal = 0;

        qulonglong total() const;
        qulonglong idleTotal() const;
    };

    std::vector<RawCpuStats> readCPUStats() const;

    double calculateCPUUsage(
        const RawCpuStats &first,
        const RawCpuStats &second) const;

    double getCPUFrequency(int cpuNumber) const;
    double getCPUTemperature() const;

    std::vector<RawCpuStats> m_previousStats;

    double m_usage = 0.0;
    double m_temperature = 0.0;
    double m_frequency = 0.0;
    int m_coreCount = 0;

    QVariantList m_cores;
    QVariantList m_history;
    QHash<QString, QVariantList> m_perCoreHistory;
    QVariantList m_coreHistory;

    QTimer m_timer;
};

#endif
