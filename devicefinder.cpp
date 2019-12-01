#include "devicefinder.h"

DeviceFinder::DeviceFinder(QObject *parent):QUdpSocket(parent)
{
    timer = new QTimer;
    if(!bind(QHostAddress::Any, Port::UDP_PORT)){
        qDebug()<<"绑定失败";
    }
    connect(this, &QUdpSocket::readyRead, this, &DeviceFinder::processDatagram);
}


DeviceFinder *DeviceFinder::getInstance()
{
    static DeviceFinder instance;
    return &instance;
}

DeviceFinder::~DeviceFinder(){
    qDebug()<<"close device finder";
    close();
}

QString DeviceFinder::getName(const QHostAddress &address) const{
    return access_point.key(address);
}

QHostAddress DeviceFinder::getAddress(const QString &name) const{
    return access_point[name];
}

void DeviceFinder::setDeviceInfo(DeviceInfo *deviceInfo)
{
    this->deviceInfo = deviceInfo;
}

void DeviceFinder::startDiscover(){
    access_point.clear();
    qDebug()<<"Start discover"<<endl;

    connect(timer, &QTimer::timeout, this, &DeviceFinder::discover);
    timer->start(1000);

}

void DeviceFinder::stopDiscover()
{
    //停止计时器即可停止时间
    timer->stop();
    disconnect(timer, &QTimer::timeout, this, &DeviceFinder::discover);
}

void DeviceFinder::setAccessPoint(QString name, QHostAddress addr)
{
    this->access_point[name] = addr;
}

/*
 * 搜索
 */
void DeviceFinder::queryFile(QString &filename)
{
    qDebug()<<"Query File:"<<filename<<endl;
    writeDatagram("[QUERY]##"+filename.toUtf8(),QHostAddress::Broadcast,Port::UDP_PORT);
}
/*
 * 搜索
 */


// 开始搜索
void DeviceFinder::discover()
{

    qDebug()<<"loop discover"<<endl;
    writeDatagram("[DISCOVER]",QHostAddress::Broadcast,Port::UDP_PORT); // 发送UDP数据报
    status = DISCOVER; // 状态调整为正在发现
    qDebug()<<"当前时间"<<QTime::currentTime().toString() <<endl<<"接入表情况:"<<access_point;
}

// 拿到接入点表
QMap<QString, QHostAddress> DeviceFinder::getAccessPoint()
{
    return access_point;
}

// 收到数据包后
void DeviceFinder::processDatagram(){
    while (hasPendingDatagrams()) {
        QNetworkDatagram datagram = receiveDatagram();
//        qDebug()<<"123";
        if (!datagram.senderAddress().isNull() && datagram.senderPort() != -1 ){
            qDebug()<<datagram.data()<<" "<<datagram.senderAddress();
            if(datagram.data() == "[DISCOVER]") {
                // 收到搜索信号
                if(deviceInfo){
                    if(datagram.senderAddress().toIPv4Address()!=deviceInfo->getLocalAddress().toIPv4Address()){
                        // 若不是自己 发送NAME个人信息
                        qDebug()<<"收到嗅探信号";
                        writeDatagram("[NAME]##"+deviceInfo->getName().toUtf8(),
                                      datagram.senderAddress(),quint16(datagram.senderPort()));
                    }
                }else{
                    qDebug()<<"Not set device info yet!";
                }
            }
            if (datagram.data().left(8) == "[NAME]##"){
                // 收到NAME个人信息
                QString name = QString::fromUtf8(datagram.data().mid(8));
                if(access_point[name].isNull()){
                    access_point[name] = datagram.senderAddress();
                    qDebug()<<"发现设备:"<<access_point;
                    emit updateDeviceList(&access_point);
                }else{
                    qDebug()<<name+"设备已存入接入表";
                }

            }
            /*
             * 搜索
             */
            if (datagram.data().left(9) == "[QUERY]##"){
                // 收到查询请求
                QString filename = QString::fromUtf8(datagram.data().mid(9));
                qDebug()<<"query path:"<<deviceInfo->getLocalSharePath()+"/"+filename;
                QFileInfo file(deviceInfo->getLocalSharePath()+"/"+filename);
                if(file.exists()){
                    // 发送存在信号
                    writeDatagram("[EXIST]##"+deviceInfo->getName().toUtf8(),datagram.senderAddress(),quint16(datagram.senderPort()));
                    // 发出“收到查询并存在信号”

                    emit recvQueryAndExist(datagram.senderAddress(),deviceInfo->getLocalSharePath()+"/"+filename);
                }else{
                    writeDatagram("[NO]##"+deviceInfo->getName().toUtf8(),datagram.senderAddress(),quint16(datagram.senderPort()));
                }
            }

            // 收到存在信号
            if(datagram.data().left(9)=="[EXIST]##"){
                if(datagram.senderAddress().toIPv4Address()!=deviceInfo->getLocalAddress().toIPv4Address()){
                    //如果不是自己
                    QString deviceName = QString::fromUtf8(datagram.data().mid(9));
                    // 发出“找到文件信号”
                    emit queryFind(deviceName);
                }


            }

            // 收到没找到信号
            if(datagram.data().left(6)=="[NO]##"){
                if(datagram.senderAddress().toIPv4Address()!=deviceInfo->getLocalAddress().toIPv4Address()){

                    //如果不是自己
                    QString deviceName = QString::fromUtf8(datagram.data().mid(9));
                    // 发出“该设备没找到文件信号”
                    emit queryNotFind(deviceName);
                }

            }
            /*
             * 搜索
             */
        }
    }
}

