#include "deviceinfo.h"

DeviceInfo::DeviceInfo(QObject* parent):QObject(parent)
{
    // 初始化设备信息
    QString localname = QHostInfo::localHostName();
    name = localname.split('.')[0];
    QHostInfo info = QHostInfo::fromName(localname);
    foreach (QHostAddress address, info.addresses()) {
        if(address.protocol()==QAbstractSocket::IPv4Protocol){
            localAddress = address;
            break;
        }
    }
    localSharePath = QDir::homePath()+"/Rayer/Shared";

    QDir dir;
    if (!dir.exists(localSharePath)) {
        dir.mkpath(localSharePath);
    }
    qDebug()<<"Your device name:"<<name<<endl
            <<"Your device IP:"<<localAddress.toString()<<endl
            <<"Your local sharing path:"<<localSharePath<<endl;
}

QString DeviceInfo::getName()
{
    return name;
}

QHostAddress DeviceInfo::getLocalAddress()
{
    return localAddress;
}

QString DeviceInfo::getLocalAddressString(){
    return localAddress.toString();
}

QString DeviceInfo::getLocalSharePath()
{
    return localSharePath;
}

DeviceInfo* DeviceInfo::getInstance(){
    static DeviceInfo info;
    return &info;
}
