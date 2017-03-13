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

    setFocus();

    m_dbPath = m_reg.value("db_path", "").toString();
    qDebug() << m_dbPath;

    if(m_dbPath.isEmpty()) {
        return;
    }

    QFileInfo info(m_dbPath);
    if(info.exists() && info.isDir()) {
        loadDB();
    }
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

bool MainWindow::loadDB()
{
    m_db.clear();

    for(int t = FontType::Start; t<FontType::End; ++t) {
        FontType::type type = static_cast<FontType::type>(t);
        bool ok = loadFontType(type);
        if(!ok) {
            m_db.clear();
            return false;
        }
    }

    /*for(int t = FontType::Start; t<FontType::End; ++t) {
        FontType::type type = static_cast<FontType::type>(t);
        qDebug() << type;
        qDebug() << m_db[type];
    }*/

    return true;
}

void MainWindow::storeDB()
{
}

void MainWindow::on_saveButton_clicked()
{

}
