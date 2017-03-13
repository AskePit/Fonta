#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QSettings>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setFocus();

    QSettings s("PitM", "Fonta DB Manager");
    m_dbPath = s.value("db_path", "").toString();

    if(m_dbPath.isEmpty()) {
        return;
    }

    QFileInfo info(m_dbPath);
    if(info.exists() && info.isFile()) {
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
    if(info.exists() && info.isDir()) {
        loadDB();
    }
}

void MainWindow::loadFontType(FontType::type t)
{
    QFile file(m_dbPath + "/" + FontType::fileName(t));

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_db.clear();
        return;
    }

    QTextStream textStream(&file);

    QStringList strings;
    while (!textStream.atEnd()) {
        strings << textStream.readLine();
    }

    file.close();

    m_db[t] = strings.toSet();
}

void MainWindow::loadDB()
{
    m_db.clear();

    for(int t = FontType::Start; t<FontType::End; ++t) {
        FontType::type type = static_cast<FontType::type>(t);
        loadFontType(type);
    }

    /*for(int t = FontType::Start; t<FontType::End; ++t) {
        FontType::type type = static_cast<FontType::type>(t);
        qDebug() << type;
        qDebug() << m_db[type];
    }*/
}

void MainWindow::storeDB()
{
}

void MainWindow::on_saveButton_clicked()
{

}
