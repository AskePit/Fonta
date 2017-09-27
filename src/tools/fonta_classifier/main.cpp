#include "mainwindow.h"
#include <QApplication>

#include <QDebug>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(QStringLiteral(":/logo")));

    MainWindow w;
    w.show();

    return a.exec();
}
