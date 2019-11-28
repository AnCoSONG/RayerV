#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QObject>
#include <QHostAddress>
#include <QHostInfo>

class DeviceInfo
{
private:
    QString name = "unknown";
    QHostAddress localAddress;
    DeviceInfo();
public:
    QString getName();
    QHostAddress getLocalAddress();
    static DeviceInfo* getInstance();
    QString getLocalAddressString();
};

#endif // DEVICEINFO_H
