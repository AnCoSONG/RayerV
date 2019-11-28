#include "deviceinfo.h"

DeviceInfo::DeviceInfo()
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
    qDebug()<<"Your device name:"<<name<<endl<<"Your device IP:"<<localAddress.toString()<<endl;
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

DeviceInfo* DeviceInfo::getInstance(){
    static DeviceInfo info;
    return &info;
}
