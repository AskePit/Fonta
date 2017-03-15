#include "mainwindow.h"
#include <QApplication>

#include "searchengine.h"
#include <QDebug>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    /*MainWindow w;
    w.show();

    return a.exec();*/

    auto families = QFontDatabase().families();
    for(CStringRef f : families) {
        qDebug() << f;
        qDebug() << trim(f);
        qDebug() << ' ';
    }

    return 0;
}

