#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_reg("PitM", "Fonta DB Manager")
{
    ui->setupUi(this);

    ui->lineEdit->setFocus();
    connectBoxes();
    connect(ui->lineEdit, &QLineEdit::editingFinished, this, &MainWindow::search);
    connect(ui->lineEdit, &QLineEdit::textEdited, this, &MainWindow::clearUi);
    connect(qApp, &QApplication::aboutToQuit, [&](){
        m_classifier.store();
    });

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

bool MainWindow::loadDB()
{
    clearUi();

    //qDebug() << "gonna load";
    bool ok = m_classifier.load(m_dbPath);

    if(ok) {
        onLoadSuccess();
    } else {
        onLoadFailure();
    }

    return ok;
}

void MainWindow::doCheckboxes(std::function<void(QCheckBox *, bool)> func, bool b)
{
    static const QList<QCheckBox *> boxes = {
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

static void setCheckboxEnabled(QCheckBox *box, bool b)
{
    box->setEnabled(b);
}

static void setCheckboxChecked(QCheckBox *box, bool b)
{
    box->setChecked(b);
}

void MainWindow::clearUi()
{
    doCheckboxes(setCheckboxChecked, false);
    ui->statusBar->clearMessage();
}

void MainWindow::onLoadSuccess()
{
    m_loaded = true;
    doCheckboxes(setCheckboxEnabled, true);
    ui->lineEdit->setEnabled(true);
    ui->statusBar->showMessage("DB loaded");
}

void MainWindow::onLoadFailure()
{
    m_loaded = true;
    doCheckboxes(setCheckboxEnabled, false);
    ui->lineEdit->setEnabled(false);
    ui->statusBar->showMessage("No DB");
}

void MainWindow::search()
{
    if(!m_loaded) {
        return;
    }

    clearUi();
    using namespace fonta;

    int info = m_classifier.fontInfo(ui->lineEdit->text());
    if(!FontType::exists(info)) {
        ui->statusBar->showMessage("No Font");
        return;
    }

    static const QMap<FontType::type, QCheckBox *> boxesMap = {
        { FontType::Serif, ui->serifBox },
        { FontType::Sans, ui->sansBox },
        { FontType::Script, ui->scriptBox },
        { FontType::Display, ui->decorativeBox },
        { FontType::Symbolic, ui->symbolBox },
        { FontType::Oldstyle, ui->oldStyleBox },
        { FontType::Transitional, ui->transitionalBox },
        { FontType::Modern, ui->modernBox },
        { FontType::Slab, ui->slabBox },
        { FontType::Grotesque, ui->grotesqueBox },
        { FontType::Geometric, ui->geometricBox },
        { FontType::Humanist, ui->humanistBox },
        { FontType::Monospaced, ui->monospacedBox },
    };

    bool found = false;
    for(cauto type : FontType::enumerate()) {
        if(info & type) {
            found = true;
            boxesMap[type]->setChecked(true);
        }
    }

    if(found) {
        ui->statusBar->showMessage("Found");
    }
}

void MainWindow::storeDB()
{
}

void MainWindow::on_saveButton_clicked()
{

}
