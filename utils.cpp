#include "utils.h"

#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>

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
