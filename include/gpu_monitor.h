
#ifndef GPU_MONITOR_H
#define GPU_MONITOR_H

#include <QObject>
#include <QTimer>
#include <QString>
#include <QVariantList>

class GpuMonitor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double usage READ usage NOTIFY updated)
    Q_PROPERTY(double temperature READ temperature NOTIFY updated)
    Q_PROPERTY(double usedVRAM READ usedVRAM NOTIFY updated)
    Q_PROPERTY(double totalVRAM READ totalVRAM NOTIFY updated)
    Q_PROPERTY(QString name READ name NOTIFY updated)
    Q_PROPERTY(bool available READ available NOTIFY updated)

    Q_PROPERTY(QVariantList history READ history NOTIFY updated)
    Q_PROPERTY(QVariantList vramHistory READ vramHistory NOTIFY updated)

public:
    explicit GpuMonitor(QObject *parent = nullptr);

    double usage() const;
    double temperature() const;
    double usedVRAM() const;
    double totalVRAM() const;
    QString name() const;
    bool available() const;

    QVariantList history() const;
    QVariantList vramHistory() const;

signals:
    void updated();

private slots:
    void refresh();

private:
    double m_usage = 0.0;
    double m_temperature = 0.0;
    double m_usedVRAM = 0.0;
    double m_totalVRAM = 0.0;

    QString m_name = "GPU unavailable";
    bool m_available = false;

    QVariantList m_history;
    QVariantList m_vramHistory;

    QTimer m_timer;
};

#endif

#ifndef GPU_MONITOR_H
#define GPU_MONITOR_H

#include <QObject>
#include <QTimer>
#include <QString>
#include <QVariantList>

class GpuMonitor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double usage READ usage NOTIFY updated)
    Q_PROPERTY(double temperature READ temperature NOTIFY updated)
    Q_PROPERTY(double usedVRAM READ usedVRAM NOTIFY updated)
    Q_PROPERTY(double totalVRAM READ totalVRAM NOTIFY updated)
    Q_PROPERTY(QString name READ name NOTIFY updated)
    Q_PROPERTY(bool available READ available NOTIFY updated)

    Q_PROPERTY(QVariantList history READ history NOTIFY updated)
    Q_PROPERTY(QVariantList vramHistory READ vramHistory NOTIFY updated)

public:
    explicit GpuMonitor(QObject *parent = nullptr);

    double usage() const;
    double temperature() const;
    double usedVRAM() const;
    double totalVRAM() const;
    QString name() const;
    bool available() const;

    QVariantList history() const;
    QVariantList vramHistory() const;

signals:
    void updated();

private slots:
    void refresh();

private:
    double m_usage = 0.0;
    double m_temperature = 0.0;
    double m_usedVRAM = 0.0;
    double m_totalVRAM = 0.0;

    QString m_name = "GPU unavailable";
    bool m_available = false;

    QVariantList m_history;
    QVariantList m_vramHistory;

    QTimer m_timer;
};

#endif
