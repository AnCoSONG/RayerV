#include "connectionreceiver.h"

ConnectionReceiver::ConnectionReceiver(QObject *parent) : QTcpServer (parent)
{
    qDebug()<<"Connection Receiver Created";
}

void ConnectionReceiver::beginListen()
{

    //开始监听
    if(!listen(QHostAddress::Any, Port::TCP_PORT)){

        qDebug()<<"监听失败";
    }
}

ConnectionReceiver::~ConnectionReceiver()
{
    qDebug()<<"Close Connection Receiver";
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




