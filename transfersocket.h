#ifndef TRANSFERSOCKET_H
#define TRANSFERSOCKET_H

#include <QTcpSocket>
#include <QHostAddress>
#include "Port.h"
class TransferSocketPrivate;
class TransferSocket : public QTcpSocket
{
    Q_OBJECT

public:
    TransferSocket();
    ~TransferSocket();

    Q_INVOKABLE bool requestNewConnection(); // 请求连接

    Q_INVOKABLE void setDestAddress(const QHostAddress &address); //设置目的地址
    Q_INVOKABLE void sendFile(const QUrl &url); // 发送文件
    Q_INVOKABLE void writeToSocket(const QByteArray &data) { QTcpSocket::write(data); } // 写数据到socket

signals:
    void hasError(const QString &error);
    void establishConnection(QString result);
    void sendFileStatus(QString status);
    void recvFileStatus(QString status);

public slots:
    void processRecvBlock(); // 处理接收到的block

private:
    TransferSocketPrivate *socketConfig;
};

#endif // TRANSFERSOCKET_H
