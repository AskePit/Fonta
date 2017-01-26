#include "types.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>

Version::Version(int major, int minor, int build)
    : major(major)
    , minor(minor)
    , build(build)
{
    const QChar dot = '.';
    str = QString::number(major);
    str += dot;
    str += QString::number(minor);
    str += dot;
    str += QString::number(build);
}

int getDPI() {
    static int dpi = QApplication::desktop()->logicalDpiX();
    return dpi;
}

int callQuestionDialog(CStringRef message)
{
    QMessageBox msgBox;
    msgBox.setText(message);

    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    return msgBox.exec();
}

void callInfoDialog(CStringRef message)
{
    QMessageBox msgBox;
    msgBox.setText(message);

    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}
