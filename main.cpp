#include <QApplication>
#include <QStyleFactory>
#include <QCommandLineParser>
#include <QIcon>
#include <QTimer>
#include <QProgressBar>
#include <QLayout>
#include <QLabel>
#include <QDialog>

#include <ctime>
#include "fontadb.h"
#include "mainwindow.h"

struct Args {
    QString file;

    Args()
        : file(QString::null)
    {}
};

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

static Args getArgs(QApplication &app)
{
    Args args;
    QCommandLineParser parser;
    parser.process(app);

    cauto list = parser.positionalArguments();
    if(!list.isEmpty()) {
        args.file = list.first();
    }

    return args;
}

int main(int argc, char *argv[])
{
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    srand(time(NULL));

    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/pic/logo.png"));

    LoadDialog d;

    // dirty hack with lambda. fontaDB emits signals from std::threads and it works for Releases for some reason
    QObject::connect(&fonta::fontaDB(), &fonta::DB::emitProgress, [&](int val){
        d.bar->setValue(val);
    });
    QObject::connect(&fonta::fontaDB(), &fonta::DB::loadFinished, &d, &QDialog::accept);
    QTimer::singleShot(250, &fonta::fontaDB(), &fonta::DB::load);

    d.exec();

    cauto args = getArgs(a);
    fonta::MainWindow w(args.file);
    w.show();

    return a.exec();
}
