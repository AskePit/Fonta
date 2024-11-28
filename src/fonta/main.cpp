#include <QApplication>
#include <QStyleFactory>
#include <QCommandLineParser>
#include <QIcon>
#include <QTimer>
#include <ctime>

#include "launcher.h"

struct Args {
    QString file;

    Args()
        : file(QString())
    {}
};

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
    using namespace fonta;

    QApplication::setStyle(QStyleFactory::create(QStringLiteral("Fusion")));
    srand(time(NULL));

    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(QStringLiteral(":/pic/logo.png")));

    cauto args = getArgs(a);

    Launcher launcher(args.file);
    QTimer::singleShot(0, &launcher, &Launcher::start); // execute launcher after a.exec() when all signal/slots mechanism and gui thread become working

    return a.exec();
}
