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

}

Rayer::~Rayer()
{
    delete ui;
}

void Rayer::init(){
    // 设备信息初始化
    info = DeviceInfo::getInstance();
    finder = DeviceFinder::getInstance();
    finder->setDeviceInfo(info);
    transfer = FileTransfer::getInstance();
    transfer->setDeviceInfo(info);
}

void Rayer::on_connectToDevice_clicked()
{
    qDebug()<<"connect to device:"<<currentSelectedDevice;
    finder->stopDiscover(); //停止嗅探
    emit statusChange("正在连接");
    transfer->setAccessPoint(currentSelectedDevice);

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
}
