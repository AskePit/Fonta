#ifndef SAMPLER_H
#define SAMPLER_H

#include "types_fonta.h"
#include <QObject>
#include <QSet>
#include <QStringList>

namespace fonta {

class WorkArea;

typedef void (*SampleLoader)(WorkArea&);

enum_class (Family) {
    Start,
    Arial = Start,
    ArialBlack,
    ArialNarrow,
    BaskervilleOldFace,
    BloggerSans,
    Bodoni,
    Calibri,
    Caslon,
    CenturyGothic,
    CenturySchoolbook,
    Chaparral,
    Clarendon,
    Coolvetica,
    CooperBlack,
    FranklinGothicBook,
    FranklinGothicDemi,
    FranklinGothicDemiCond,
    Futura,
    Garamond,
    Georgia,
    GillSans,
    GillSansCondenced,
    Helvetica,
    Impact,
    NotoSans,
    Tahoma,
    TimesNewRoman,
    Trebuchet,
    SegoeUI,
    Verdana,
    End

    enum_interface

    static const QMap<type, QStringList> familyMap;
    static bool exists(type t);
    static QString name(type t);
};

struct Sample {
    Family::type family1;
    int size1;
    Family::type family2;
    int size2;
};

class Sampler : public QObject
{
    Q_OBJECT
public:
    static Sampler *instance();

    CStringRef getName();
    CStringRef getEngText(ContentMode mode);
    CStringRef getRusText(ContentMode mode);
    void loadSample(WorkArea& area);

private slots:
    void fetchNewsSlot();

private:
    Sampler();
    Sampler(const Sampler &) = delete;
    void operator=(const Sampler &) = delete;
    static Sampler *mInstance;

    void fetchNews(QStringList &list, CStringRef url, CStringRef tag);

    static const QStringList names;
    static QStringList textsEng;
    static QStringList textsRus;
    static const QVector<Sample> preSamples;
    static QVector<Sample> samples;

    static QSet<int> namesPool;
    static QSet<int> textsEngPool;
    static QSet<int> textsRusPool;
    static QSet<int> samplesPool;
};

} // namespace fonta

#endif // SAMPLER_H
