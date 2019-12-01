#include "filetransfer.h"
#include <QThread>
#include<QCoreApplication>

FileTransfer *FileTransfer::getInstance()
{
    static FileTransfer ft;
    return &ft;
}

FileTransfer::FileTransfer(QObject *parent) : QObject(parent)
{
    server = new ConnectionReceiver(this); //创建服务器
    server->beginListen(); //开始监听
    connect(server, &ConnectionReceiver::hasNewConnection, this, [this](QTcpSocket *socket) {
        if (client)
            client->deleteLater();
        client = qobject_cast<TransferSocket *>(socket);
    });
    QDir dir;
    if (!dir.exists(QCoreApplication::applicationDirPath() + "/FileRecv/")) {
        dir.mkpath(QCoreApplication::applicationDirPath() + "/FileRecv/");
    }
    qDebug()<<QCoreApplication::applicationDirPath() + "/FileRecv/";
}

FileTransfer::~FileTransfer(){
    qDebug()<<"closing file transfer";
    delete server;
//    delete info;
    if(!threadLists.isEmpty()){
        int i = 0;
        foreach (QThread* l, threadLists) {
            l->deleteLater();
            qDebug()<< ++i;
        }

    }
    qDebug()<<"Closed";

}

void FileTransfer::setAccessPoint(const QString &name){ //界面上只显示名字，因此需要从名字拿到ip地址
    DeviceFinder *df = DeviceFinder::getInstance();
    QHostAddress address = df->getAddress(name);
    qDebug()<<address;

    QThread *thread = new QThread;
    threadLists.append(thread);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    TransferSocket *socket = new TransferSocket;
    socket->moveToThread(thread);
    thread->start();
    client = socket;
    QMetaObject::invokeMethod(client, "setDestAddress", Q_ARG(QHostAddress, address));
}


void FileTransfer::sendFile(const QUrl &url){
    // 这两行是做界面的，暂时不需要
//    QFileInfo info(QQmlFile::urlToLocalFileOrQrc(url));
//    FileManager::instance()->addWriteFile(info.fileName(), int(info.size()));
    QMetaObject::invokeMethod(client, "sendFile", Q_ARG(QUrl, url)); // 不太懂为什么需要QMetaObject调用
    //    client->sendFile(url);
}

void FileTransfer::setDeviceInfo(DeviceInfo *info)
{
    this->info = info;
}

TransferSocket *FileTransfer::getTransferSocket()
{
    return client;
}

ConnectionReceiver *FileTransfer::getConnectionReceiver()
{
    return server;
}
