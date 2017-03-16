#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setContentsMargins(0,0,0,0);
    ui->horizontalLayout->setContentsMargins(0,0,0,0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_dBox_valueChanged(int arg1)
{
    ui->canvas->d = arg1;
    ui->canvas->update();
}

void MainWindow::on_nBox_valueChanged(int arg1)
{
    ui->canvas->n = arg1;
    ui->canvas->update();
}

void MainWindow::on_hBox_valueChanged(int arg1)
{
    ui->canvas->h = arg1;
    ui->canvas->update();
}

void MainWindow::on_wBox_valueChanged(int arg1)
{
    ui->canvas->w = arg1;
    ui->canvas->update();
}

void MainWindow::on_aBox_valueChanged(int arg1)
{
    ui->canvas->a = arg1;
    ui->canvas->update();
}
