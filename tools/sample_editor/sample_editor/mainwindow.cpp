#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "info.h"
#include "ui_info.h"

#include <QCheckBox>
#include <QResizeEvent>
#include <QDirIterator>
#include <QFileDialog>
#include <QSettings>
#include <QClipboard>
#include <QDebug>

#define checked        (state == Qt::Checked)
#define unchecked      (state == Qt::Unchecked)
#define semiState      (state == Qt::PartiallyChecked)
#define maybeChecked   (checked || semiState)

#define bindWeight(X) \
    connect(ui->X##Box, &QCheckBox::stateChanged, [&](int state){ \
        ui->X##Weight->setEnabled(semiState); \
        if(semiState) { \
            ui->X##Weight->setValue(50); \
        } \
        ui->X##Weight->setValue(checked ? 100 : unchecked ? 0 : ui->X##Weight->value()); \
        update(); \
    }); \
    connect(ui->X##Weight, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&](int value){ \
        if(value == 0) { \
            ui->X##Box->setChecked(false); \
        } \
        \
        if(value == 100) { \
            ui->X##Box->setChecked(true); \
        } \
        update(); \
    })

#define bindGroup(X) \
    connect(ui->X##Box, &QCheckBox::stateChanged, [&](int state) { \
        ui->X##Group->setEnabled(maybeChecked); \
        update(); \
    })

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    infoDialog(nullptr),
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

   double ratio = 1200./300.;
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
    if(pos >= 0 && pos < files.length()) {
        QSettings s("PitM", "Fonta Sample Editor");
        s.setValue("filename", files[pos]);
    }

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
    files.clear();
    ui->backButton->setDisabled(true);
    ui->nextButton->setDisabled(true);

    QFileInfo info(filename);
    QString dirName = info.path();

    QDirIterator dirIt(dirName, {"*.png"}, QDir::Files, QDirIterator::Subdirectories);

    while(dirIt.hasNext()) {
        dirIt.next();
        files << dirIt.filePath();
    };

    if(files.count()) {
        pos = files.indexOf(filename);
        if(pos != -1) {
            loadCurrSample();
        } else {
            loadPrevNextSample(Direction::Forward);
        }
    }
}

void MainWindow::loadPrevNextSample(Direction direction)
{
    direction == Direction::Forward ? ++pos : --pos;
    loadCurrSample();
}

void MainWindow::loadCurrSample()
{
    if(pos < 0) {
        pos = 0;
        ui->backButton->setDisabled(true);
        return;
    }

    if(pos >= files.count()) {
        pos = files.count()-1;
        ui->nextButton->setDisabled(true);
        return;
    }

    const QString &filename = files[pos];
    QFileInfo info(filename);
    currConfig = QString("%1/%2.ini").arg(info.path(), info.completeBaseName());
    readConfig();

    ui->image->setPixmap(filename);
    ui->statusBar->showMessage(info.baseName());
    QApplication::clipboard()->setText(info.baseName());

    ui->nextButton->setEnabled(pos < files.count());
    ui->backButton->setEnabled(pos > 0);
    update();
}

void MainWindow::on_nextButton_clicked()
{
    saveConfig();
    loadPrevNextSample(Direction::Forward);

    setFocus();
}

void MainWindow::on_backButton_clicked()
{
    saveConfig();
    loadPrevNextSample(Direction::Backward);

    setFocus();
}

void MainWindow::readConfig()
{
    if(currConfig.isEmpty()) {
        return;
    }

#define readTristate(X) \
    ui->X##Box->setCheckState(static_cast<Qt::CheckState>(s.value(#X, Qt::Unchecked).toInt())); \
    ui->X##Weight->setValue(s.value(#X"Weight", 0).toInt())

    QSettings s(currConfig, QSettings::IniFormat);

    s.beginGroup("FamilyType");
    readTristate(serif);
    readTristate(sans);
    readTristate(script);
    readTristate(decorative);
    readTristate(symbol);
    s.endGroup();

    s.beginGroup("SerifStyle");
    readTristate(oldStyle);
    readTristate(transitional);
    readTristate(modern);
    readTristate(slab);
    s.endGroup();

    s.beginGroup("SansStyle");
    readTristate(grotesque);
    readTristate(geometric);
    readTristate(humanist);
    s.endGroup();

    s.beginGroup("Other");
    ui->monospacedBox->setChecked(s.value("monospaced", false).toBool());
    s.endGroup();

    return;

#undef saveTristate
}

void MainWindow::saveConfig()
{
    if(currConfig.isEmpty()) {
        return;
    }

#define saveTristate(X) \
    s.setValue(#X, static_cast<int>(ui->X##Box->checkState())); \
    s.setValue(#X"Weight", ui->X##Weight->value())

    QSettings s(currConfig, QSettings::IniFormat);

    s.beginGroup("FamilyType");
    saveTristate(serif);
    saveTristate(sans);
    saveTristate(script);
    saveTristate(decorative);
    saveTristate(symbol);
    s.endGroup();

    s.beginGroup("SerifStyle");
    saveTristate(oldStyle);
    saveTristate(transitional);
    saveTristate(modern);
    saveTristate(slab);
    s.endGroup();

    s.beginGroup("SansStyle");
    saveTristate(grotesque);
    saveTristate(geometric);
    saveTristate(humanist);
    s.endGroup();

    s.beginGroup("Other");
    s.setValue("monospaced", ui->monospacedBox->isChecked());
    s.endGroup();
#undef saveTristate

    return;
}

void MainWindow::on_saveButton_clicked()
{
    saveConfig();
    loadPrevNextSample(Direction::Forward);
}

static bool isFileDone(const QString &filename)
{
    if(filename.isEmpty()) {
        return false;
    }

    QSettings s(filename, QSettings::IniFormat);
    s.beginGroup("State");
    bool done = s.value("done", false).toBool();
    s.endGroup();

    return done;
}

void MainWindow::on_actionInfo_triggered()
{
    if(!infoDialog) {
        infoDialog = new Info(this);
    }

    infoDialog->ui->list->clear();

    int count = 0;
    for(const auto &f : files) {
        QFileInfo info(f);
        QString iniFile = QString("%1/%2.ini").arg(info.path(), info.completeBaseName());
        if(!isFileDone(iniFile)) {
            ++count;
            infoDialog->ui->list->addItem(info.baseName());
        }
    }

    infoDialog->ui->countLabel->setText(QString::number(count));
    infoDialog->show();
}
