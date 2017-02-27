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
    QProgressBar *bar;
};

LoadDialog::LoadDialog()
{
    setWindowFlags(Qt::CustomizeWindowHint);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    //QLabel *lbl = new QLabel(tr("Fonts load:"));

    bar = new QProgressBar;
    bar->setMinimumHeight(50);
    bar->setMinimumWidth(100);
    bar->setWindowFlags(Qt::Widget);
    bar->setRange(0, 100);
    bar->setValue(0);

    //mainLayout->addWidget(lbl);
    mainLayout->addWidget(bar);

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

    QObject::connect(&fontaDB(), &DB::emitProgress, d.bar, &QProgressBar::setValue);
    QObject::connect(&fontaDB(), &DB::loadFinished, &d, &QDialog::accept, Qt::QueuedConnection);

    QTimer::singleShot(50, &fontaDB(), &DB::load);

    d.exec();

    MainWindow *w = new MainWindow(m_fileToOpen);
    w->show();
}

} // namespace fonta
