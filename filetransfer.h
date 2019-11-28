#ifndef FILETRANSFER_H
#define FILETRANSFER_H

#include <QFileInfo>
#include <QObject>
#include "transfersocket.h"
#include "connectionreceiver.h"
#include "devicefinder.h"
// 文件传输的管理类
// 负责发送文件
class FileTransfer : public QObject
{
    Q_OBJECT

public:
    static FileTransfer* getInstance();
    Q_INVOKABLE void setAccessPoint(const QString &name); //设置
    Q_INVOKABLE void sendFile(const QUrl &url);
//    Q_INVOKABLE bool buildConnection();
    void setDeviceInfo(DeviceInfo* info);
    TransferSocket *getTransferSocket();
    ConnectionReceiver *getConnectionReceiver();
private:
    FileTransfer(QObject *parent = nullptr);
    ~FileTransfer();
    DeviceInfo* info;
    TransferSocket* client;
    ConnectionReceiver* server;


signals:

public slots:
};

#endif // FILETRANSFER_H
