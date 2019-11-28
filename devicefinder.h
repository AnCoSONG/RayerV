#ifndef DEVICEFINDER_H
#define DEVICEFINDER_H

#include <QObject>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QTimer>
#include <QTime>
#include "Port.h"
#include "deviceinfo.h"

class DeviceFinder : public QUdpSocket
{
    // UDP广播器，发现局域网内的设备
    Q_OBJECT

public:
    static DeviceFinder* getInstance();

    ~DeviceFinder();
    QString getName(const QHostAddress &address) const;
    QHostAddress getAddress(const QString &name) const;
    void setDeviceInfo(DeviceInfo* deviceInfo);
    void discover();
    QMap<QString, QHostAddress> getAccessPoint();
    void startDiscover();
    void stopDiscover();
    void setAccessPoint(QString name, QHostAddress addr);

signals:
    void updateDeviceList(QMap<QString, QHostAddress> *access_points);

private:
    enum STATUS{ WAIT, DISCOVER };  // 2种状态建立连接
    STATUS status = WAIT;
    DeviceFinder(QObject *parent = nullptr);
    DeviceInfo* deviceInfo = nullptr;
    static DeviceFinder instance;
    void processDatagram();
    QMap<QString, QHostAddress> access_point;
    QTimer* timer;
};

#endif // DEVICEFINDER_H
