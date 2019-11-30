#include "filemangaer.h"


FileMangaer::FileMangaer(QObject *parent):QObject(parent)
{
    qDebug()<<"FileManager created";
}

FileMangaer *FileMangaer::getInstance()
{
    static FileMangaer f;
    return &f;
}

void FileMangaer::addRecvFile(QString filename, QString filesize, QString status)
{
    emit recv_new_file(filename, filesize, status);
}

void FileMangaer::updateRecvFile(QString filename, QString status)
{
    emit update_recv_file(filename, status);
}
