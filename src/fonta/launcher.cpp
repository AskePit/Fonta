#include "launcher.h"

#include "fontadb.h"
#include "mainwindow.h"

#include <QProgressBar>
#include <QLayout>
#include <QLabel>
#include <QDialog>
#include <QTimer>

namespace fonta {

class LoadDialog : public QDialog
{
public:
    LoadDialog();
};

LoadDialog::LoadDialog()
{
    setWindowFlags(Qt::CustomizeWindowHint);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    QLabel *lbl = new QLabel(tr("Loading..."));

    lbl->setMinimumHeight(60);
    lbl->setMinimumWidth(120);
    lbl->setWindowFlags(Qt::Widget);
    lbl->setAlignment(Qt::AlignCenter);

    lbl->setStyleSheet("QLabel { background-color: rgb(250, 250, 250); }");

    mainLayout->addWidget(lbl);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(mainLayout);
}

Launcher::Launcher(CStringRef fileToOpen, QObject *parent)
    : QObject(parent)
    , m_fileToOpen(fileToOpen)
{}

void Launcher::start()
{
    // At first call dialog with fonts load progress bar
    LoadDialog d;
    QObject::connect(&fontaDB(), &DB::loadFinished, &d, &QDialog::accept, Qt::QueuedConnection);
    QTimer::singleShot(200, &fontaDB(), &DB::load);

    d.exec();

    MainWindow *w = new MainWindow(m_fileToOpen);
    connect(qApp, &QApplication::aboutToQuit, [=](){ w->deleteLater(); });
    w->show();
}

} // namespace fonta
