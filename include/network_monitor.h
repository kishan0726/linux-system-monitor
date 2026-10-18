
#ifndef NETWORK_MONITOR_H
#define NETWORK_MONITOR_H

#include <QObject>
#include <QTimer>
#include <QString>

class NetworkMonitor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double downloadMBps READ downloadMBps NOTIFY updated)
    Q_PROPERTY(double uploadMBps READ uploadMBps NOTIFY updated)
    Q_PROPERTY(QString interfaceName READ interfaceName NOTIFY updated)
    Q_PROPERTY(bool available READ available NOTIFY updated)

public:
    explicit NetworkMonitor(QObject *parent = nullptr);

    double downloadMBps() const;
    double uploadMBps() const;
    QString interfaceName() const;
    bool available() const;

signals:
    void updated();

private slots:
    void refresh();

private:
    double m_downloadMBps = 0.0;
    double m_uploadMBps = 0.0;

    QString m_interfaceName = "N/A";
    bool m_available = false;

    qulonglong m_previousRxBytes = 0;
    qulonglong m_previousTxBytes = 0;
    bool m_hasPreviousSample = false;

    QTimer m_timer;
};

#endif
