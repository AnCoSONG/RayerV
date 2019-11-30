#ifndef FILEMANGAER_H
#define FILEMANGAER_H

#include <QObject>
#include <QDebug>

class FileMangaer:public QObject
{
    // 负责界面更新
    Q_OBJECT

//    Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY filenameChanged)
//    Q_PROPERTY(QString filesize READ filesize WRITE setFilesize NOTIFY filesizeChanged)
//    Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)
private:
    FileMangaer(QObject* parent=nullptr);
public:
    static FileMangaer* getInstance();

    Q_INVOKABLE void addRecvFile(QString filename, QString filesize, QString status);

    Q_INVOKABLE void updateRecvFile(QString filename, QString status);

signals:
    void recv_new_file(QString filename, QString filesize, QString status);

    void update_recv_file(QString filename, QString status);

};

#endif // FILEMANGAER_H
