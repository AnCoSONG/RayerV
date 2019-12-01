#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QObject>
#include <QHostAddress>
#include <QHostInfo>
#include <QDir>

class DeviceInfo:public QObject
{
    Q_OBJECT
private:
    QString name = "unknown";
    QHostAddress localAddress;
    QString localSharePath;
    DeviceInfo(QObject* parent = nullptr);
public:
    QString getName();
    QHostAddress getLocalAddress();
    static DeviceInfo* getInstance();
    QString getLocalAddressString();
    QString getLocalSharePath();
};

#endif // DEVICEINFO_H
