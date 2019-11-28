#ifndef CONNECTIONRECEIVER_H
#define CONNECTIONRECEIVER_H

#include <QTcpServer>
#include <QThread>
#include "Port.h"
#include "transfersocket.h"
// 接收连接
class ConnectionReceiver : public QTcpServer
{
    Q_OBJECT
public:
    ConnectionReceiver(QObject *parent=nullptr);
    void beginListen();

signals:
    void hasNewConnection(QTcpSocket *socket);

protected:
    virtual void incomingConnection(qintptr handle);
};

#endif // CONNECTIONRECEIVER_H
