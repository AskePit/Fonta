#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QCommandLineParser>
#include <QProcess>
#include <ctime>
#include "sampler.h"

struct Args {
    QString file;

    Args()
        : file(QString::null)
    {}
};

Args getArgs(QApplication &app)
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

    cauto args = getArgs(a);

    fonta::Sampler::instance();
    fonta::MainWindow w(args.file);
    w.show();

    return a.exec();
}
