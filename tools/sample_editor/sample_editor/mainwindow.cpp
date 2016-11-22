#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCheckBox>
#include <QResizeEvent>
#include <QDirIterator>
#include <QFileDialog>

#define checked        (state == Qt::Checked)
#define unchecked      (state == Qt::Unchecked)
#define semiState      (state == Qt::PartiallyChecked)
#define maybeChecked   (checked || semiState)

#define bindWeight(BOOL_NAME) \
    connect(ui->BOOL_NAME##Box, &QCheckBox::stateChanged, [&](int state){ \
        ui->BOOL_NAME##Weight->setEnabled(semiState); \
        ui->BOOL_NAME##Weight->setValue(checked ? 100 : unchecked ? 0 : ui->BOOL_NAME##Weight->value()); \
    })

#define bindGroup(BOOL_NAME) \
    connect(ui->BOOL_NAME##Box, &QCheckBox::stateChanged, [&](int state) { \
        ui->BOOL_NAME##Group->setEnabled(maybeChecked); \
    })

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    pos(-1)
{
    ui->setupUi(this);

    bindGroup(serif);
    bindGroup(sans);

    bindWeight(serif);
    bindWeight(sans);
    bindWeight(script);
    bindWeight(decorative);
    bindWeight(symbol);

    bindWeight(oldStyle);
    bindWeight(transitional);
    bindWeight(modern);
    bindWeight(slab);

    bindWeight(grotesque);
    bindWeight(geometric);
    bindWeight(humanist);

    ui->image->setScaledContents(true);
    ui->image->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    openDir("../../font_samples");

    setFocus();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);

   int ratio = 1600/400;
   QSize s = event->size();
   const int margin = 25;
   int height = s.width()/(double)ratio;
   ui->image->setMaximumWidth(s.width()-margin);
   ui->image->setMaximumHeight(height);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QMainWindow::keyPressEvent(event);

    if(event->key() == Qt::Key_Left) {
        if(ui->backButton->isEnabled()) {
            on_backButton_clicked();
        }
    } else if(event->key() == Qt::Key_Right) {
        if(ui->nextButton->isEnabled()) {
            on_nextButton_clicked();
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QString dirName = QFileDialog::getExistingDirectory();
    if(dirName.isEmpty()) {
        return;
    }

    openDir(dirName);
}

void MainWindow::openDir(const QString &dirName)
{
    files.clear();
    ui->backButton->setDisabled(true);
    ui->nextButton->setDisabled(true);

    QDirIterator dirIt(dirName, {"*.png"}, QDir::Files, QDirIterator::Subdirectories);
    while(dirIt.hasNext()) {
        dirIt.next();
        files << dirIt.filePath();
    }

    if(files.count()) {
        pos = 0;
        getSample();
    }
}

void MainWindow::getSample()
{
    QString filename = files[pos];
    QFileInfo info(filename);

    ui->image->setPixmap(filename);
    ui->statusBar->showMessage(info.fileName());
    ui->nextButton->setEnabled(pos < files.count());
    ui->backButton->setEnabled(pos > 0);
}

void MainWindow::on_nextButton_clicked()
{
    ++pos;
    getSample();

    setFocus();
}

void MainWindow::on_backButton_clicked()
{
    --pos;
    getSample();

    setFocus();
}
