#include <QGuiApplication>
#include "sampler.h"

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    Sampler sampler;
    sampler.createSamples();

    return 0;
}
