#include "connectionreceiver.h"

ConnectionReceiver::ConnectionReceiver(QObject *parent) : QTcpServer (parent)
{
    qDebug()<<"Connection Receiver Created";
}

void ConnectionReceiver::beginListen()
{
    listen(QHostAddress::Any, Port::TCP_PORT); //开始监听
}

void ConnectionReceiver::incomingConnection(qintptr handle){
    QThread *thread = new QThread;
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    TransferSocket *socket = new TransferSocket;
    if (!socket->setSocketDescriptor(handle)) {
        qDebug() << "Socket Error: " << socket->errorString();
    } else {
        emit hasNewConnection(socket);
        qDebug() << "Connected Socket: " << handle;
    }
    socket->moveToThread(thread);
    thread->start();
}




