#include "sampler.h"

#include <QImage>
#include <QPainter>
#include <QDebug>
#include <QFontDatabase>

Sampler::Sampler(SamplerSettings settings)
{
#define copy(X) this->X = settings.X
    copy(boxSize);
    copy(fontSize);
    copy(dir);
    copy(chars);
    copy(columns);
    copy(ext);
#undef copy
}

void Sampler::addChar(cstring family, QChar c, QVector<QImage*> &images)
{
    QImage *image_ptr = new QImage(boxSize, boxSize, QImage::Format_Mono);
    QImage &image = *image_ptr;

    image.setColor(0, qRgb(255, 255, 255));
    image.setColor(1, qRgb(0, 0, 0));
    image.fill(Qt::white);

    QFont f(family);
    f.setPixelSize(fontSize);
    f.setStyleStrategy(QFont::NoAntialias);


    QPainter painter(&image);
    painter.setFont(f);
    painter.drawText(0, boxSize, QString(c));

    int topY = 0;
    for(int y = 0; y<image.height(); ++y) {
        for(int x = 0; x<image.width(); ++x) {
            if(image.pixel(x, y) == qRgb(0, 0, 0)) {
                topY = y;
                goto btmY;
            }
        }
    }

btmY:
    int bottomY = boxSize;
    for(int y = image.height()-1; y>=0; --y) {
        for(int x = 0; x<image.width(); ++x) {
            if(image.pixel(x, y) == qRgb(0, 0, 0)) {
                bottomY = y;
                goto lftX;
            }
        }
    }

lftX:
    int leftX = 0;
    for(int x = 0; x<image.width(); ++x) {
        for(int y = 0; y<image.height(); ++y) {
            if(image.pixel(x, y) == qRgb(0, 0, 0)) {
                leftX = x;
                goto rghtX;
            }
        }
    }

rghtX:
    int rightX = boxSize;
    for(int x = image.width()-1; x>=0; --x) {
        for(int y = 0; y<image.height(); ++y) {
            if(image.pixel(x, y) == qRgb(0, 0, 0)) {
                rightX = x;
                goto end;
            }
        }
    }

end:

    int width = rightX-leftX+1;
    int height = bottomY-topY+1;

    image.fill(Qt::white);
    painter.drawText((boxSize-width)/2, boxSize-(boxSize-height)/2, QString(c));

    images << image_ptr;
}

void Sampler::createSample(cstring family)
{
    QFileInfo info(QString("%1/%2.%3").arg(dir, family, ext));
    if(info.exists()) {
        return;
    }

    const int rows = ceil(chars.count()/(double)columns);
    QImage dest(columns*boxSize, rows*boxSize, QImage::Format_Mono);
    dest.setColor(0, qRgb(255, 255, 255));
    dest.setColor(1, qRgb(0, 0, 0));
    dest.fill(Qt::white);

    QVector<QImage*> images;
    for(const auto &c : chars) {
        addChar(family, c, images);
    }

    QPainter painter(&dest);
    int x = 0;
    int y = 0;
    for(const auto image : images) {
        painter.drawImage(x, y, *image);
        x = (x+boxSize);
        if(x >= columns*boxSize) {
            x = 0;
            y += boxSize;
        }
    }

    QDir().mkpath(dir);
    dest.save(info.filePath());

    qDebug() << family;
}

void Sampler::createSamples()
{
    for(const auto &family : QFontDatabase().families()) {
        createSample(family);
    }
    qDebug() << "end";
}
