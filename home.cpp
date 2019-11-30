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
    connect(transfer->getConnectionReceiver(),&ConnectionReceiver::hasNewConnection, this, &Rayer::on_hasNewConnection);
    connect(manager, &FileMangaer::recv_new_file, this, &Rayer::on_add_recv_file);
    connect(manager, &FileMangaer::update_recv_file, this, &Rayer::on_update_recv_file);

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
    finder->stopDiscover(); //停止嗅探
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
    qDebug()<<"停止嗅探";
    finder->stopDiscover();
    emit statusChange("已停止嗅探");
    ui->startDiscover->setText("开始嗅探");
    emit statusChange("已被动建立连接");
    ui->startDiscover->setChecked(false);
}

void Rayer::on_establishConnection(QString res)
{
    if(res=="GG"){
        emit statusChange("建立连接失败");
    }else if(res=="Yeah"){
        qDebug()<<"停止嗅探";
        finder->stopDiscover();
        emit statusChange("已停止嗅探");
        ui->startDiscover->setText("开始嗅探");
        emit statusChange("已连接:"+currentSelectedDevice);
        ui->startDiscover->setChecked(false);

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
    // 添加
    int row = ui->recvFileList->rowCount();
    ui->recvFileList->insertRow(row);
    QTableWidgetItem *filenameItem = new QTableWidgetItem(filename);
    QTableWidgetItem *filesizeItem = new QTableWidgetItem(filesize);
    QTableWidgetItem *statusItem = new QTableWidgetItem(status);

    ui->recvFileList->setItem(row, 0, filenameItem);
    ui->recvFileList->setItem(row, 1, filesizeItem);
    ui->recvFileList->setItem(row, 2, statusItem);

}

void Rayer::on_update_recv_file(QString filename, QString status)
{
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
    QProcess process;
    QString filepath = qApp->applicationDirPath() + "/FileRecv/" +  ui->recvFileList->item(row, 0)->text();
    qDebug()<<qApp->platformName();

    if(qApp->platformName()=="windows"){
        filepath.replace("/","\\");
        qDebug()<<filepath;
        process.startDetached("explorer /select," + filepath);
    }

}
