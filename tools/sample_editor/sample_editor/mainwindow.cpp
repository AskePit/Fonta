#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCheckBox>
#include <QResizeEvent>
#include <QDirIterator>
#include <QFileDialog>
#include <QSettings>
#include <QClipboard>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setFocus();

    QSettings s("PitM", "Fonta Sample Editor");
    QString filename = s.value("filename", "").toString();
    if(!filename.isEmpty()) {
        openFile(filename);
    }
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QMainWindow::keyPressEvent(event);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName();
    if(filename.isEmpty()) {
        return;
    }

    openFile(filename);
}

void MainWindow::openFile(const QString &filename)
{

}

void MainWindow::readConfig()
{

}

void MainWindow::saveConfig()
{
}

void MainWindow::on_saveButton_clicked()
{
    saveConfig();
}
