#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>

static void setCheckboxEnabled(QCheckBox *box, bool b)
{
    box->setEnabled(b);
}

static void setCheckboxChecked(QCheckBox *box, bool b)
{
    box->setChecked(b);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_reg("PitM", "Fonta DB Manager")
{
    ui->setupUi(this);

    setFocus();
    connectBoxes();
    connect(ui->lineEdit, &QLineEdit::editingFinished, this, &MainWindow::search);

    m_dbPath = m_reg.value("db_path", "").toString();
    qDebug() << m_dbPath;

    if(m_dbPath.isEmpty()) {
        return;
    }

    ui->statusBar->showMessage("No DB");

    QFileInfo info(m_dbPath);
    if(info.exists() && info.isDir()) {
        loadDB();
    }
}

void MainWindow::connectBoxes()
{
    auto makeConnect = [&](QCheckBox *subj, QCheckBox *affected) {
        connect(subj, &QCheckBox::toggled, [=](bool toggled) {
            if(!toggled) { return; }
            if(affected->isChecked()) {
                affected->setChecked(false);
            }
        });
    };

    makeConnect(ui->serifBox, ui->oldStyleBox);
    makeConnect(ui->serifBox, ui->transitionalBox);
    makeConnect(ui->serifBox, ui->modernBox);
    makeConnect(ui->serifBox, ui->slabBox);

    makeConnect(ui->sansBox, ui->grotesqueBox);
    makeConnect(ui->sansBox, ui->geometricBox);
    makeConnect(ui->sansBox, ui->humanistBox);

    makeConnect(ui->oldStyleBox, ui->serifBox);
    makeConnect(ui->transitionalBox, ui->serifBox);
    makeConnect(ui->modernBox, ui->serifBox);
    makeConnect(ui->slabBox, ui->serifBox);

    makeConnect(ui->grotesqueBox, ui->sansBox);
    makeConnect(ui->geometricBox, ui->sansBox);
    makeConnect(ui->humanistBox, ui->sansBox);
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
    m_dbPath = QFileDialog::getExistingDirectory(this, "Choose Fonta DB folder", "");
    if(m_dbPath.isEmpty()) {
        return;
    }

    QFileInfo info(m_dbPath);
    if(!info.exists() || !info.isDir()) {
        return;
    }

    bool ok = loadDB();
    if(ok) {
        m_reg.setValue("db_path", m_dbPath);
    }
}
/*
bool MainWindow::loadFontType(FontType::type t)
{
    QFile file(m_dbPath + "/" + FontType::fileName(t));

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream textStream(&file);

    QStringList strings;
    while (!textStream.atEnd()) {
        strings << textStream.readLine();
    }

    file.close();

    m_db[t] = strings.toSet();
    return true;
}
*/
bool MainWindow::loadDB()
{
    /*m_db.clear();
    clearUi();

    for(int t = FontType::Start; t<FontType::End; ++t) {
        FontType::type type = static_cast<FontType::type>(t);
        bool ok = loadFontType(type);
        if(!ok) {
            m_db.clear();
            onLoadFailure();
            return false;
        }
    }*/

    /*for(int t = FontType::Start; t<FontType::End; ++t) {
        FontType::type type = static_cast<FontType::type>(t);
        qDebug() << type;
        qDebug() << m_db[type];
    }*/

    onLoadSuccess();
    return true;
}

void MainWindow::doCheckboxes(std::function<void(QCheckBox *, bool)> func, bool b)
{
    static QList<QCheckBox *> boxes = {
        ui->oldStyleBox,
        ui->transitionalBox,
        ui->modernBox,
        ui->slabBox,
        ui->grotesqueBox,
        ui->geometricBox,
        ui->humanistBox,
        ui->serifBox,
        ui->sansBox,
        ui->scriptBox,
        ui->decorativeBox,
        ui->symbolBox,
        ui->monospacedBox,
    };

    for(QCheckBox *box : boxes) {
        func(box, b);
    }
}

void MainWindow::clearUi()
{
    ui->infoLabel->setText("");

    doCheckboxes(setCheckboxChecked, false);
    ui->statusBar->showMessage("");
}

void MainWindow::onLoadSuccess()
{
    doCheckboxes(setCheckboxEnabled, true);
    ui->statusBar->showMessage("DB loaded");
}

void MainWindow::onLoadFailure()
{
    doCheckboxes(setCheckboxEnabled, false);
    ui->statusBar->showMessage("No DB");
}




void MainWindow::search()
{
    clearUi();



    ui->statusBar->showMessage("No such font in DB");
}

void MainWindow::storeDB()
{
}

void MainWindow::on_saveButton_clicked()
{

}
