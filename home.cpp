#include "home.h"
#include "ui_home.h"

Rayer::Rayer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Rayer)
{
    ui->setupUi(this);
    this->setFixedSize(800, 600);
    this->setWindowTitle(tr("Rayer"));

    init();


    connect(this,&Rayer::statusChange,this,&Rayer::on_statusChange);
    connect(finder, &DeviceFinder::updateDeviceList, this,&Rayer::on_updateDeviceList);
    /*
     * 搜索
     */
    connect(finder,&DeviceFinder::queryFind, this,&Rayer::on_queryFound);
    connect(finder, &DeviceFinder::queryNotFind, this, &Rayer::on_queryNotFound);
    connect(finder, &DeviceFinder::recvQueryAndExist, this, &Rayer::on_recvQueryAndExist);
    /*
     * 搜索
     */
    connect(transfer->getConnectionReceiver(),&ConnectionReceiver::hasNewConnection, this, &Rayer::on_hasNewConnection);
    connect(manager, &FileMangaer::recv_new_file, this, &Rayer::on_add_recv_file,Qt::QueuedConnection);
    connect(manager, &FileMangaer::update_recv_file, this, &Rayer::on_update_recv_file,Qt::QueuedConnection);

}

Rayer::~Rayer()
{
    delete ui;
//    qDebug()<<1;
//    delete info;
//    qDebug()<<2;
//    delete finder;
//    qDebug()<<3;
//    delete manager;
//    qDebug()<<4;
//    delete transfer;
//    qDebug()<<5;
}

void Rayer::init(){
    // 设备信息初始化
    info = DeviceInfo::getInstance();
    finder = DeviceFinder::getInstance();
    finder->setDeviceInfo(info);
    transfer = FileTransfer::getInstance();
    transfer->setDeviceInfo(info);
    manager = FileMangaer::getInstance();
}

void Rayer::on_connectToDevice_clicked()
{
    qDebug()<<"connect to device:"<<currentSelectedDevice;
    ui->startDiscover->setChecked(false);
    emit statusChange("正在连接");
    transfer->setAccessPoint(currentSelectedDevice);

    connect(transfer->getTransferSocket(), &TransferSocket::establishConnection, this, &Rayer::on_establishConnection);
    connect(transfer->getTransferSocket(), &TransferSocket::sendFileStatus, this, &Rayer::on_sendFileStatusChange);
//    connect(transfer->getTransferSocket(), &TransferSocket::recvFileStatus,this, &Rayer::on_recvFileStatusChange);

}

void Rayer::on_chooseFile_clicked()
{
    qDebug()<<"Choose File";
    QUrl selectedUrl = QFileDialog::getOpenFileUrl(this, tr("选择一个您要发送的文件"),QDir::rootPath(),tr("All Files(*.*)"));
    if(selectedUrl.isEmpty()){
        qDebug()<<"请重新选择";
        ui->pathText->setText("请重新选择");
        return;
    }
    ui->pathText->setText(selectedUrl.toLocalFile());
    fileURL = selectedUrl;
}

void Rayer::on_sendFile_clicked()
{
    qDebug()<<"Send File Start";
    transfer->sendFile(fileURL);
}

void Rayer::on_startDiscover_toggled(bool checked)
{

    if(checked){
        qDebug()<<"开始嗅探";
        finder->startDiscover();
        emit statusChange("正在嗅探");
        ui->startDiscover->setText("停止嗅探");
        ui->deviceList->clear(); //开始嗅探就清除

    }else{
        qDebug()<<"停止嗅探";
        finder->stopDiscover();
        emit statusChange("已停止嗅探");
        ui->startDiscover->setText("开始嗅探");

    }
}

void Rayer::on_statusChange(QString newStatus)
{
    ui->deviceStatus->setText(newStatus);
}

void Rayer::on_updateDeviceList(QMap<QString, QHostAddress> * access_point)
{
    qDebug()<<"正在更新列表";
    QMap<QString, QHostAddress>::iterator it = access_point->begin();
    ui->deviceList->clear();
    while(it!=access_point->end()){
        qDebug()<<"名称: "<<it.key();
        qDebug()<<"地址: "<<it.value();
        // 把名字赋给listwidget
        ui->deviceList->addItem(it.key());
        it++;
    }

}

void Rayer::on_deviceList_itemClicked(QListWidgetItem *item)
{
    qDebug()<<item->text();
    currentSelectedDevice = item->text();
}

void Rayer::on_hasNewConnection(QTcpSocket *socket)
{
    ui->startDiscover->setChecked(false);
    emit statusChange("已被动建立连接");

}

void Rayer::on_establishConnection(QString res)
{
    if(res=="GG"){
        emit statusChange("建立连接失败");
    }else if(res=="Yeah"){
        ui->startDiscover->setChecked(false);
        emit statusChange("已连接:"+currentSelectedDevice);


    }
}

void Rayer::on_sendFileStatusChange(QString status)
{
    qDebug()<<"发送状态更新";
    if(status!="Done"){
        emit statusChange(status);
    }else {
        emit statusChange("已连接:"+currentSelectedDevice);
        QMessageBox::information(nullptr, "成功","已发送"); // 发送成功弹出确认。
    }


}

void Rayer::on_add_recv_file(QString filename, QString filesize, QString status)
{
    qDebug()<<"Filename: "<<filename<<endl
            <<"Filesize: "<<filesize<<endl
            <<"Status: "<<status<<endl;
    if(ui->recvFileList->findItems(filename,Qt::MatchExactly).isEmpty()){
        // 是新的
        // 添加
        int row = ui->recvFileList->rowCount();
        ui->recvFileList->insertRow(row);
        QTableWidgetItem *filenameItem = new QTableWidgetItem(filename);
        QTableWidgetItem *filesizeItem = new QTableWidgetItem(filesize);
        QTableWidgetItem *statusItem = new QTableWidgetItem(status);

        ui->recvFileList->setItem(row, 0, filenameItem);
        ui->recvFileList->setItem(row, 1, filesizeItem);
        ui->recvFileList->setItem(row, 2, statusItem);
    }else{
        on_update_recv_file(filename, status);
    }


}

void Rayer::on_update_recv_file(QString filename, QString status)
{
//    QString lfn = filename.toLocal8Bit();
//    QString ls = status.toLocal8Bit();
    qDebug()<<"Filename: "<<filename<<endl
            <<"Updated Status: "<<status<<endl;

    // 更新
    QTableWidgetItem* item = ui->recvFileList->findItems(filename,Qt::MatchExactly)[0];
    ui->recvFileList->setItem(item->row(), 2, new QTableWidgetItem(status));


}

//void Rayer::on_recvFileStatusChange(QString status)
//{
//    qDebug()<<"接收状态更新";
//    emit statusChange(status);
//}

void Rayer::on_recvFileList_cellDoubleClicked(int row, int column)
{
    QUrl path;
//    QProcess process;
    QString filepath = qApp->applicationDirPath() + "/FileRecv/" +  ui->recvFileList->item(row, 0)->text();
    qDebug()<<qApp->platformName();

    if(qApp->platformName()=="windows"){
//        filepath.replace("/","\\");
        qDebug()<<filepath;
        filepath = "file:///"+filepath;
//        process.startDetached("explorer /select," + filepath);
        QDesktopServices::openUrl(filepath);
    }else if(qApp->platformName()=="cocoa"){
        filepath = "file:///"+filepath;
        QDesktopServices::openUrl(filepath);
    }

}

void Rayer::on_deviceList_itemDoubleClicked(QListWidgetItem *item)
{
    qDebug()<<item->text();
    currentSelectedDevice = item->text();
    on_connectToDevice_clicked();
}

void Rayer::on_openRecvPath_clicked()
{
    QProcess process;
    QString recvpath = qApp->applicationDirPath() + "/FileRecv";
    qDebug()<<recvpath;
    qDebug()<<qApp->platformName();
    if(qApp->platformName()=="windows"){
        recvpath.replace("/","\\");
        qDebug()<<recvpath;
        process.startDetached("explorer /e,"+recvpath);
    }else if(qApp->platformName()=="cocoa"){
        process.startDetached("open "+recvpath);
    }

}

void Rayer::on_about_clicked()
{
    QMessageBox::information(this,"关于","计网项目\n宋晋瑜制作\n支付宝:18512855406\n欢迎转账.");
}

/*
 * 搜索
 */

void Rayer::on_searchAndDownload_clicked()
{
    if(!filenameTobeSearch.isEmpty()){
        qDebug()<<"要搜索的文件:"<<filenameTobeSearch;
        finder->queryFile(filenameTobeSearch);
        QMessageBox::information(this,"已发送","已广播查询请求\n文件名:"+filenameTobeSearch);
    }else{
        QMessageBox::warning(this,"警告","请正确输入搜索文件名后再重试");
    }
}

void Rayer::on_searchLine_textChanged(const QString &arg1)
{
    filenameTobeSearch = arg1;
    qDebug()<<filenameTobeSearch;
}

void Rayer::on_queryNotFound(QString deviceName)
{
    QMessageBox::information(this,"未找到","设备:"+deviceName+"的共享文件夹未找到文件");
}

void Rayer::on_queryFound(QString deviceName)
{
    QMessageBox::information(this,"已找到","设备:"+deviceName+"的共享文件夹中找到了文件");
}

void Rayer::on_recvQueryAndExist(QHostAddress address,QString filepath)
{
    finder->setAccessPoint("Query Device",address);
    transfer->setAccessPoint("Query Device");
    transfer->sendFile(QUrl(filepath));

}

/*
 * 搜索
 */
