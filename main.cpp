#include "home.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<QHostAddress>("QHostAddress");
    Rayer w;
    w.show();
    return a.exec();
}
