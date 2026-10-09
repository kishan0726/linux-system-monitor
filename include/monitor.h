
#ifndef MONITOR_H
#define MONITOR_H

#include <QObject>
#include <QJsonObject>
#include <QTimer>

class Monitor : public QObject
{
    Q_OBJECT

public:
    explicit Monitor(QObject *parent = nullptr);

signals:
    void metricsUpdated(const QJsonObject &metrics);

private slots:
    void collectMetrics();

private:
    QTimer timer;
};

#endif
