#ifndef SAMPLER_H
#define SAMPLER_H

#include <QDir>

using cstring = const QString &;

struct SamplerSettings {
    int boxSize;
    int fontSize;
    QString dir;
    QString chars;
    int columns;
    QString ext;

    SamplerSettings() {
        boxSize = 150;
        fontSize = 120;
        dir = "../../font_samples";
        chars = "QRagAxHTEfSkMOCq";
        columns = 8;
        ext = "png";
    }
};

class Sampler
{
public:
    Sampler(SamplerSettings settings = SamplerSettings());
    void createSample(cstring family);
    void createSamples();

private:
    int boxSize;
    int fontSize;
    QString dir;
    QString chars;
    int columns;
    QString ext;

    void addChar(cstring family, QChar c, QVector<QImage*> &images);
};

#endif // SAMPLER_H
