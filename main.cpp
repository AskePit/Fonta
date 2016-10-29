#include "fontawindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <ctime>
#include "sampler.h"

int main(int argc, char *argv[])
{
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    srand(time(NULL));

    QApplication a(argc, argv);
    Sampler::initSamples();
    FontaWindow w;
    w.show();

    return a.exec();
}
