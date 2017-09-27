#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "importdialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_reg(QStringLiteral("PitM"), QStringLiteral("Fonta DB Manager"))
{
    ui->setupUi(this);

    m_boxesMap = {
        { fonta::FontType::Serif, ui->serifBox },
        { fonta::FontType::Sans, ui->sansBox },
        { fonta::FontType::Script, ui->scriptBox },
        { fonta::FontType::Display, ui->decorativeBox },
        { fonta::FontType::Symbolic, ui->symbolBox },
        { fonta::FontType::Oldstyle, ui->oldStyleBox },
        { fonta::FontType::Transitional, ui->transitionalBox },
        { fonta::FontType::Modern, ui->modernBox },
        { fonta::FontType::Slab, ui->slabBox },
        { fonta::FontType::Grotesque, ui->grotesqueBox },
        { fonta::FontType::Geometric, ui->geometricBox },
        { fonta::FontType::Humanist, ui->humanistBox },
        { fonta::FontType::Monospaced, ui->monospacedBox },
    };

    ui->lineEdit->setFocus();
    connectBoxes();
    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &MainWindow::search);
    connect(ui->lineEdit, &QLineEdit::textEdited, this, &MainWindow::clearUi);
    connect(qApp, &QApplication::aboutToQuit, [&]() {
        m_classifier.store();
    });

    m_dbPath = m_reg.value(QStringLiteral("db_path"), QStringLiteral("")).toString();
    qDebug() << m_dbPath;

    if(m_dbPath.isEmpty()) {
        return;
    }

    ui->statusBar->showMessage(tr("No DB"));

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
    m_dbPath = QFileDialog::getExistingDirectory(this, tr("Choose Fonta DB folder"), QStringLiteral(""));
    if(m_dbPath.isEmpty()) {
        return;
    }

    QFileInfo info(m_dbPath);
    if(!info.exists() || !info.isDir()) {
        return;
    }

    bool ok = loadDB();
    if(ok) {
        m_reg.setValue(QStringLiteral("db_path"), m_dbPath);
    }
}

bool MainWindow::loadDB()
{
    clearUi();

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
    m_found = false;
}

void MainWindow::onLoadSuccess()
{
    m_loaded = true;
    doCheckboxes(setCheckboxEnabled, true);
    ui->lineEdit->setEnabled(true);
    ui->statusBar->showMessage(tr("DB loaded"));
}

void MainWindow::onLoadFailure()
{
    m_loaded = true;
    doCheckboxes(setCheckboxEnabled, false);
    ui->lineEdit->setEnabled(false);
    ui->statusBar->showMessage(tr("No DB"));
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
        ui->statusBar->showMessage(tr("No Font"));
        return;
    }

    m_found = false;
    for(cauto type : FontType::enumerate()) {
        if(info & type) {
            m_found = true;
            m_boxesMap[type]->setChecked(true);
        }
    }

    if(m_found) {
        ui->statusBar->showMessage(tr("Found"));
    }
}

static int callQuestionDialog(CStringRef message)
{
    QMessageBox msgBox;
    msgBox.setText(message);

    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    return msgBox.exec();
}

void MainWindow::on_saveButton_clicked()
{
    if(m_found) {
        int ret = callQuestionDialog(tr("Rewrite existed font info?"));
        if (ret != QMessageBox::Ok) {
            return;
        }
    }

    int info = 0;
    for(cauto type : fonta::FontType::enumerate()) {
        if(m_boxesMap[type]->isChecked()) {
            info |= type;
        }
    }

    if(m_found) {
        m_classifier.rewriteFontInfo(ui->lineEdit->text(), info);
    } else {
        m_classifier.addFontInfo(ui->lineEdit->text(), info);
    }
    ui->statusBar->showMessage(tr("Saved"));
}

void MainWindow::on_actionImport_triggered()
{
    ImportDialog d;
    int response = d.exec();

    if (response == QDialog::Rejected) {
        return;
    }

    QFile file(d.fileName());

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ui->statusBar->showMessage(tr("Error: could not open import file"));
        return;
    }

    QTextStream textStream(&file);
    QStringList list;
    while (!textStream.atEnd()) {
        list << textStream.readLine();
    }

    file.close();

    list.removeDuplicates();
    list.removeOne(QStringLiteral(""));

    int info = d.info();
    for(CStringRef family : list) {
        m_classifier.addFontInfo(family, info);
    }

    ui->statusBar->showMessage(tr("File Imported"));
}
