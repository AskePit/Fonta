#ifndef SAMPLER_H
#define SAMPLER_H

#include <QSet>
#include <QStringList>
#include <fontawidgets.h>

typedef void (*SampleLoader)(FontaWorkArea&);

class Sampler;

struct Sample {
    QString family1;
    int size1;
    QString family2;
    int size2;
};

class Sampler : public QObject
{
    Q_OBJECT
public:
    static Sampler *instance();

    static CStringRef getName();
    static CStringRef getText();
    static CStringRef getRusText();
    static CStringRef getTextForFamily(CStringRef family);
    static void loadSample(FontaWorkArea& area);

private slots:
    void fetchNewsSlot();

private:
    Sampler();
    Sampler(const Sampler &) = delete;
    void operator=(const Sampler &) = delete;
    static Sampler *mInstance;

    void fetchNews(QStringList &list, CStringRef url, CStringRef tag);

    static const QStringList names;
    static QStringList texts;
    static QStringList textsRus;
    static const QVector<Sample> preSamples;
    static QVector<Sample> samples;


    static QSet<int> namesPool;
    static QSet<int> textsPool;
    static QSet<int> textsRusPool;
    static QSet<int> samplesPool;
};

#endif // SAMPLER_H
