
#ifndef NETWORK_MONITOR_H
#define NETWORK_MONITOR_H

#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <QElapsedTimer>
#include <QString>
#include <QtGlobal>

class NetworkMonitor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool available READ available NOTIFY updated)
    Q_PROPERTY(QString interfaceName READ interfaceName NOTIFY updated)

    Q_PROPERTY(double downloadMBps READ downloadMBps NOTIFY updated)
    Q_PROPERTY(double uploadMBps READ uploadMBps NOTIFY updated)

    Q_PROPERTY(qulonglong receivedBytes READ receivedBytes NOTIFY updated)
    Q_PROPERTY(qulonglong sentBytes READ sentBytes NOTIFY updated)

    Q_PROPERTY(QVariantList downloadHistory READ downloadHistory NOTIFY updated)
    Q_PROPERTY(QVariantList uploadHistory READ uploadHistory NOTIFY updated)

public:
    explicit NetworkMonitor(QObject *parent = nullptr);

    bool available() const;
    QString interfaceName() const;

    double downloadMBps() const;
    double uploadMBps() const;

    qulonglong receivedBytes() const;
    qulonglong sentBytes() const;

    QVariantList downloadHistory() const;
    QVariantList uploadHistory() const;

signals:
    void updated();

private slots:
    void refresh();

private:
    QString defaultInterface() const;

    bool m_available = false;
    QString m_interfaceName = "Unavailable";

    double m_downloadMBps = 0.0;
    double m_uploadMBps = 0.0;

    qulonglong m_receivedBytes = 0;
    qulonglong m_sentBytes = 0;

    qulonglong m_previousReceivedBytes = 0;
    qulonglong m_previousSentBytes = 0;
    bool m_firstSample = true;

    QVariantList m_downloadHistory;
    QVariantList m_uploadHistory;

    QElapsedTimer m_elapsed;
    QTimer m_timer;
};

#endif
