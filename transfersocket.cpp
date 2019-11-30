#include "fileblock.h"
#include "transfersocket.h"

#include <QtConcurrent>
#include <QFile>
#include <QFileInfo>
#include <QHostAddress>

static const int maxBlockSize = 1024;

typedef struct
{
    QFile *file; // 文件指针
    qint32 size; // 读入的文件大小
} RayerFile;

class TransferSocketPrivate
{
public:
    int maskTaskNum = 8;
    QString cachePath;
    QByteArray recvData;
    QMap<QString, RayerFile> recvFiles;
    QHostAddress destAddress;
};

TransferSocket::TransferSocket()
{
    socketConfig = new TransferSocketPrivate;
    socketConfig->cachePath = qApp->applicationDirPath() + "/FileRecv/";
    QDir dir;
    if (!dir.exists(socketConfig->cachePath)) {
        dir.mkpath(socketConfig->cachePath);
    }

    connect(this, &QTcpSocket::readyRead, this, [this]() {
        socketConfig->recvData += readAll();
        processRecvBlock();
    });
}

TransferSocket::~TransferSocket()
{
    delete socketConfig;
}

bool TransferSocket::requestNewConnection()
{
    abort();
    if(socketConfig->destAddress.isNull()){
        qDebug()<<"还没有设置目的地址";
        return false;
    }
    connectToHost(socketConfig->destAddress, Port::TCP_PORT);
    return waitForConnected(5000);
}

void TransferSocket::setDestAddress(const QHostAddress &address)
{
    if (socketConfig->destAddress != address)
        socketConfig->destAddress = address;
    qDebug()<<"transferSocket "<<socketConfig->destAddress;
    //提示正在连接
    bool result = requestNewConnection();
    qDebug()<<"连接建立:"<< (result?"成功":"失败,请自行重试");
    // 连接情况
    // 发出连接成功或者失败信号提示界面。
    if(result){
        emit establishConnection("Yeah");
    }else {
        emit establishConnection("GG");
    }



}

void TransferSocket::sendFile(const QUrl &url)
{
    if (state() != SocketState::ConnectedState){
        qDebug()<<"未建立连接，正在重试";
        requestNewConnection();
    }

    //显示文件发送出的信号
    //更新界面
    emit sendFileStatus("开始发送");

    //并发发文件
    QtConcurrent::run([this, url]() {



        qDebug()<<"开始传送文件"<<url.toLocalFile();
        QTime time;
        time.start();
        QFile file(url.toLocalFile()); // 拿到路径
//        QFile file(QQmlFile::urlToLocalFileOrQrc(url)); // QML界面
        file.open(QIODevice::ReadOnly);

        qint32 offset = 0;
        qint32 totalSize = qint32(file.size());
        QString fileName = QFileInfo(url.toLocalFile()).fileName(); // 从toString修改
//        QString fileNameS = QFileInfo(url.toString()).fileName();
//        qDebug()<<fileName<<" "<<fileNameS;
//        qDebug()<<"fileName:"<<fileName<<"\nlocal8bit"<<fileName.toLocal8Bit()<<endl<<fileName.toUtf8();
        while (offset < totalSize) {
            file.seek(offset);
            QByteArray dataBlock = file.read(maxBlockSize);
            FileBlock block = { qint16(dataBlock.size()), offset, totalSize,
                                fileName.toUtf8(), dataBlock};
            QByteArray data;
            QDataStream out(&data, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_5_13);
            out << block;
//            this->writeToSocket()
            QMetaObject::invokeMethod(this, "writeToSocket", Q_ARG(QByteArray, data));

            offset += dataBlock.size();
            if (time.elapsed() >= 1000 || offset >= totalSize) {
                time.restart();
//                QMetaObject::invokeMethod(FileManager::instance(), "updateWriteFile",
//                                          Q_ARG(QString, fileName), Q_ARG(int, offset));
            }
        }

        file.close();
        qDebug()<<"发送完毕";
        // 显示发送完毕的信号
        emit sendFileStatus("Done");
    });

}

void TransferSocket::processRecvBlock()
{
    static QTime time = QTime::currentTime();
    if(socketConfig->recvData.size() > 0 ){
        FileBlock block;
        QDataStream in(&socketConfig->recvData, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_5_13);
        in >> block;
        if(block.isEmpty()){
//            qDebug()<<"接收完毕";
            //emit recvFileStatus("接收完成");
            return; //说明读完了，这是递归读取结束的标志
        }

        QString fileName = QString::fromUtf8(block.fileName);

        if(!socketConfig->recvFiles[fileName].file){
            //如果是新接收的文件
            QFile *file = new QFile(socketConfig->cachePath+fileName);
            file->open(QIODevice::WriteOnly);
            socketConfig->recvFiles[fileName].file = file;
            socketConfig->recvFiles[fileName].size = 0;

            // 此处 通知界面更新
//            emit recvFileStatus("开始接收");
            QMetaObject::invokeMethod(FileMangaer::getInstance(),"addRecvFile", Q_ARG(QString, fileName),
                                      Q_ARG(QString, QString::number(block.fileSize))
                                      ,Q_ARG(QString, QString::number(socketConfig->recvFiles[fileName].size)));




            QThread::msleep(100); // 线程休眠100ms再接收下一个文件块

        }

        if ( socketConfig->recvFiles[fileName].size < block.fileSize ){
            // 当前读入量比总量小，即还没读完
            socketConfig->recvFiles[fileName].size += block.blockSize;
            socketConfig->recvFiles[fileName].file->write(block.dataBlock);
            //qDebug()<<block; //当前block信息打印出来
//            emit recvFileStatus(QString::number(socketConfig->recvFiles[fileName].size));
            QMetaObject::invokeMethod(FileMangaer::getInstance(),"updateRecvFile", Q_ARG(QString, fileName)
                                      ,Q_ARG(QString,
                                             QString::number(
                                                 socketConfig->recvFiles[fileName].size)));

        }

        if (socketConfig->recvFiles[fileName].size == block.fileSize){
            // 读取完成
            socketConfig->recvFiles[fileName].file->close(); // 关闭文件指针
            socketConfig->recvFiles[fileName].file->deleteLater();
            socketConfig->recvFiles.remove(fileName);
            // 通知界面更新 必须用MetaObject::invokeMethod
//            emit recvFileStatus("接收完成");
            QMetaObject::invokeMethod(FileMangaer::getInstance(),"updateRecvFile", Q_ARG(QString, fileName)
                                      ,Q_ARG(QString, "已完成"));
        }

//        if(time.elapsed() >= 1000 ){
//            // 通知进度更新
//            time.restart();

//        }

        socketConfig->recvData.remove(0, block.size());
        if ( socketConfig->recvData.size() > 0 ){
            processRecvBlock(); // 如果没处理完则继续处理
        }

    }
}
