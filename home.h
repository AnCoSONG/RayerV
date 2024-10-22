#ifndef HOME_H
#define HOME_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <QUrl>
#include <QDir>
#include <QStringList>
#include <QListWidgetItem>
#include "filemangaer.h"
#include "Port.h"
#include "deviceinfo.h"
#include "devicefinder.h"
#include "filetransfer.h"
#include <QDesktopServices>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui { class Rayer; }
QT_END_NAMESPACE

class Rayer : public QMainWindow
{
    Q_OBJECT
private:
    typedef struct {
        QString filename;
        QString filesize;
        QString transfer_status;
    } RECV_FILE;
    QUrl fileURL;
    QStringList deviceNames;
    QList<RECV_FILE> recv_files;


    // 功能对象
    DeviceInfo *info;
    DeviceFinder *finder;
    FileTransfer* transfer;
    FileMangaer* manager;
    QString currentSelectedDevice;

    QString filenameTobeSearch;


    Ui::Rayer *ui;

private slots:

    void on_connectToDevice_clicked();

    void on_chooseFile_clicked();

    void on_sendFile_clicked();

    void on_startDiscover_toggled(bool checked);

    void on_statusChange(QString newStatus);

    void on_updateDeviceList(QMap<QString, QHostAddress>*);

    void on_deviceList_itemClicked(QListWidgetItem *item);

    void on_hasNewConnection(QTcpSocket *socket);

    void on_establishConnection(QString res);

    void on_sendFileStatusChange(QString status);

//    void on_recvFileStatusChange(QString status);

    void on_add_recv_file(QString filename, QString filesize, QString status); //接收文件信号

    void on_update_recv_file(QString filename, QString status); // 更新进度

    void on_recvFileList_cellDoubleClicked(int row, int column);

    void on_deviceList_itemDoubleClicked(QListWidgetItem *item);

    void on_openRecvPath_clicked();

    void on_about_clicked();
    /*
     * 搜索
     */

    void on_searchAndDownload_clicked();

    void on_searchLine_textChanged(const QString &arg1);

    void on_queryNotFound(QString deviceName); // 搜索找到
    void on_queryFound(QString deviceName); // 搜索没有找到
    void on_recvQueryAndExist(QHostAddress address,QString filepath); // 收到搜索请求并找到文件

    /*
     * 搜索
     */

    void on_addSharedFile_clicked();

    void on_openSharedDir_clicked();

public:
    Rayer(QWidget *parent = nullptr);
    ~Rayer();
    void init();
    int test = 0;
//    void addDeviceUI(QString dName);
//    void addRecvFileUI(RECV_FILE file);


    // 现在需要实现一些界面相关的东西
    // 1/ 界面的数据动态更新
    // 2/ 功能整合
    // 3/ 接收文件进度的实现
    // 4/ 设备的动态嗅探

signals:
    void statusChange(QString newStatus);
};
#endif // HOME_H
