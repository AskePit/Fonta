#include "sampler.h"

#include "widgets/workarea.h"

#include <cstdlib>
#include <QVector>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QXmlStreamReader>

#ifdef FONTA_MEASURES
#include <QElapsedTimer>
#include <QDebug>
#endif

#include "fontadb.h"

namespace fonta {

Sampler *Sampler::mInstance = nullptr;

Sampler *Sampler::instance() {
    if (mInstance == nullptr) {
        mInstance = new Sampler;
    }

    return mInstance;
}

Sampler::Sampler()
{
    // try to fetch rss news
    fetchNews(textsEng, "http://feeds.bbci.co.uk/news/world/rss.xml", "description");
    fetchNews(textsRus, "http://tass.ru/rss/v2.xml", "title");

    // filter font pair depending on users installed fonts
    const QStringList& families = fontaDB().families();

    for(const Sample& p : preSamples) {
        if(families.contains(p.family1) && families.contains(p.family2)) {
            samples << p;
        }
    }
}

const QStringList Sampler::names = {
    "Severin",
    "Alois",
    "Teo",
    "Tess",
    "Noel",
    "Noah",
    "Liam",
    "Alice",
    "Bob",
    "Aske",
    "Olga",
    "Tilda",
    "Vespa",
    "Solly",
    "Pit",
    "Kurt",
    "Sharona",
    "Melissa",
};

QStringList Sampler::textsEng = {
    "Before 1960 95% of soft drinks sold in the U.S. are furnished in reusable bottles."
    "Ernest Hemmingway commits suicide with shotgun.",
    "American U-2 spy plane, piloted by Francis Gary Powers, shot down over Russia",
    "Kennedy was assassinated in Dallas, Texas, on November 22, 1963",
    "Donald Trump promises to dissolve his Trump Foundation charity, which is still under investigation.",
};

QStringList Sampler::textsRus = {
    "Шифровальщица попросту забыла ряд ключевых множителей и тэгов",
    "Широкая электрификация южных губерний даст мощный толчок подъёму сельского хозяйства",
    "Подъём с затонувшего эсминца легкобьющейся древнегреческой амфоры сопряжён с техническими трудностями",
};

const QVector<Sample> Sampler::preSamples = {
    { "Georgia", 22, "Verdana", 11 },
    { "Helvetica", 26, "Garamond", 12 },
    { "Bodoni MT", 24, "FuturaLight", 16 },
    { "Bodoni MT", 24, "Futura Bk BT", 16 },
    { "Bodoni MT", 24, "Futura PT", 16 },
    { "Bodoni MT", 24, "Futura", 16 },
    { "Trebuchet MS", 18, "Verdana", 9 },
    { "Century Schoolbook", 21, "Century Gothic", 12 },
    { "Franklin Gothic Demi Cond", 24, "Century Gothic", 12 },
    { "Tahoma", 18, "Segoe UI", 11 },
    { "Franklin Gothic Demi", 20, "Trebuchet MS", 12 },
    { "Arial Black", 17, "Arial", 11 },
    { "Impact", 21, "Arial Narrow", 12 },
    { "Georgia", 18, "Calibri", 12 },
    { "Segoe UI", 20, "Arial", 11 },
    { "Clarendon", 17, "Times New Roman", 12 },
    { "Clarendon", 20, "Chaparral Pro", 14 },
    { "Cooper Black", 22, "Centaur", 17 },
    { "Helvetica", 21, "Garamond", 15 },
    { "Bodoni MT", 24, "Gill Sans MT", 16 },
    { "Bodoni MT", 20, "Gill Sans MT", 14 },
    { "Gill Sans MT", 20, "Centaur", 16 },
    { "Centaur", 20, "Franklin Gothic Book", 12 },
    { "Caslon", 20, "Franklin Gothic Book", 12 },
    { "Baskerville Old Face", 19, "Franklin Gothic Book", 11 },
    { "Gill Sans MT Condensed", 20, "Arial", 10 },
};

struct NewsData {
    QStringList *list;
    QString tag;

    NewsData()
        : list(nullptr)
        , timer(nullptr)
    {}
    NewsData(QStringList *list, const QString &tag)
        : list(list)
        , tag(tag)
#ifdef FONTA_MEASURES
        , timer(new QElapsedTimer)
#endif
    {}

#ifdef FONTA_MEASURES
    QElapsedTimer *timer;
#endif
};

QNetworkAccessManager *network;
static QHash<QNetworkReply *, NewsData> newsMap;

void Sampler::fetchNews(QStringList &list, CStringRef url, CStringRef tag)
{
    if(newsMap.isEmpty()) {
        network = new QNetworkAccessManager;
    }

    QNetworkReply *reply = network->get(QNetworkRequest(url));

    NewsData d(&list, tag);
    newsMap[reply] = d;

#ifdef FONTA_MEASURES
    d.timer->start();
#endif

    QObject::connect(reply, SIGNAL(finished()), this, SLOT(fetchNewsSlot()));
}

void cleanupNetwork(QNetworkReply *reply)
{
    reply->deleteLater();
    delete newsMap[reply].timer;
    newsMap.remove(reply);
    if(newsMap.isEmpty()) {
        delete network;
    }
}

void Sampler::fetchNewsSlot()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    NewsData &d = newsMap[reply];

    if(reply->error() != QNetworkReply::NoError) {
#ifdef FONTA_MEASURES
        qDebug() << d.timer->elapsed() << "ms: timeout to load news";
#endif
        cleanupNetwork(reply);
        return;
    } else {
#ifdef FONTA_MEASURES
        qDebug() << d.timer->elapsed() << "ms to load news";
#endif
    }

    QStringList &list = *d.list;
    CStringRef tag = d.tag;

    QStringList tmpList;

#ifdef FONTA_MEASURES
    d.timer->start();
#endif

    QXmlStreamReader r(reply->readAll());
    while(r.readNextStartElement());
    while(!r.atEnd()) {
        r.readNext();
        if(r.name() == "item") {
            while(!r.atEnd()) {
                r.readNext();
                if(r.name() == tag) {
                    tmpList << r.readElementText();
                    break;
                }
            }
        }
    }

    if(tmpList.count()) {
        list.clear();
        list << tmpList;
    }

#ifdef FONTA_MEASURES
    qDebug() << d.timer->elapsed() << "ms to process news rss-xml";
#endif

    cleanupNetwork(reply);
}

QVector<Sample> Sampler::samples;
QSet<int> Sampler::namesPool;
QSet<int> Sampler::textsEngPool;
QSet<int> Sampler::textsRusPool;
QSet<int> Sampler::samplesPool;

static int getPoolsValue(QSet<int>& pool, int length)
{    
    int r = rand()%length;

    if(!pool.contains(r)) {
        pool.insert(r);
        return r;
    } else {
        int i = r+1;
        while(i != r){
            if(i >= length) {
                i = 0;
                continue;
            }

            if(pool.contains(i)) {
                ++i;
            } else {
                pool.insert(i);
                return i;
            }
        }

        pool.clear();
        pool.insert(r);
        return r;
    }
}

static const QString engPangram = "The quick brown fox jumps over the lazy dog. 1234567890";
static const QString rusPangram = "Съешь же ещё этих мягких французских булок да выпей чаю. The quick brown fox jumps over the lazy dog. 1234567890";

static const QString engLoremIpsum =
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Praesent nisi elit, imperdiet at rhoncus ultricies, semper non leo. Sed venenatis sem eu dolor molestie pellentesque. Aenean viverra ligula vel mollis imperdiet. Curabitur quis lacus placerat, porta justo a, maximus neque. Mauris mattis luctus neque, vitae porttitor nunc dignissim eget. Curabitur nec enim augue. Praesent lectus nibh, mollis vel ligula sed, molestie dapibus odio. Donec eu nisl lobortis, sodales erat sit amet, lobortis tellus. Vestibulum iaculis sed ligula in porta. Aenean non nunc vitae eros auctor consequat ac ut libero. Praesent tristique a sapien quis porttitor. Nam interdum nisi luctus metus eleifend eleifend. Ut vehicula, sapien sit amet porttitor posuere, magna nibh mattis sem, a facilisis lectus purus et augue.\n\n"
        "Vestibulum sed diam imperdiet, finibus mauris quis, efficitur massa. Phasellus ornare blandit pellentesque. Maecenas ex lectus, semper ut aliquam a, ullamcorper at leo. Nullam consectetur tempor iaculis. Morbi dignissim velit ac lorem condimentum pharetra. Etiam eget purus ac mi porta condimentum. Vestibulum ultricies ullamcorper erat, a blandit lorem accumsan a. Aliquam pretium pellentesque magna nec varius. Aliquam posuere risus in tellus condimentum, id dapibus turpis condimentum. Nunc fermentum orci ac mollis fringilla.\n\n"
        "Aenean diam neque, bibendum a urna at, accumsan volutpat nunc. Nulla ullamcorper molestie efficitur. Nam urna nibh, tempor ornare quam sed, tincidunt blandit elit. Duis iaculis pretium lectus, ut imperdiet purus dictum in. Suspendisse condimentum porttitor rutrum. Proin id tellus quis turpis convallis imperdiet. Cras fringilla feugiat odio, sed scelerisque justo consectetur in. Curabitur at sem et sapien sodales tincidunt. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Nulla sit amet ullamcorper velit. Morbi ut metus ac lacus finibus dignissim. Nam a mi ac mi consequat molestie in sit amet odio. Cras ornare lorem nec est faucibus luctus.\n\n"
        "Nullam convallis ante nec enim sollicitudin fermentum. Aliquam id porttitor elit, vitae volutpat arcu. Nunc ultricies sapien purus, sed aliquet lorem consectetur sed. Vivamus at lectus ullamcorper, tristique odio vitae, varius dui. Aliquam tempor auctor erat sit amet mollis. Morbi in elit vitae massa imperdiet semper et nec dolor. Suspendisse potenti. Praesent viverra arcu id leo egestas, sed iaculis nibh congue. Fusce eu finibus metus. Duis id suscipit nisl, non hendrerit purus. Etiam eu lobortis velit. Nam aliquet nisi in iaculis dignissim.\n\n"
        "Phasellus consectetur, massa id mollis porttitor, lorem quam placerat magna, nec accumsan nibh nunc id nisi. Aenean porta id ex id hendrerit. Vestibulum porta interdum rhoncus. In malesuada leo sem, a congue sem tempor nec. In hac habitasse platea dictumst. Praesent interdum vehicula urna, et dictum velit malesuada non. Cras eget pharetra ante. Integer suscipit in arcu laoreet pulvinar. Integer nec ex faucibus mi elementum dapibus. Phasellus blandit nisi nec neque hendrerit, eu pellentesque ante eleifend. Mauris sit amet neque vel magna egestas mattis. Nunc magna mauris, aliquam dictum tincidunt in, tempor vitae ex. Nam placerat nunc quis aliquet blandit. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos.\n\n";

static const QString rusLoremIpsum =
        "Идейные соображения высшего порядка, а также сложившаяся структура организации позволяет выполнять важные задания по разработке систем массового участия. Идейные соображения высшего порядка, а также рамки и место обучения кадров влечет за собой процесс внедрения и модернизации существенных финансовых и административных условий. Главным образом укрепление и развитие структуры влечет за собой процесс внедрения и модернизации систем массового участия. Товарищи! рамки и место обучения кадров в значительной степени обуславливает создание позиций, занимаемых участниками в отношении поставленных задач. Разнообразный и богатый опыт начало повседневной работы по формированию позиции позволяет оценить значение форм развития. Задача организации, в особенности же рамки и место обучения кадров обеспечивает широкому кругу (специалистов) участие в формировании форм развития.\n\n"
        "Идейные соображения высшего порядка, а также рамки и место обучения кадров играет важную роль в формировании направлений прогрессивного развития. Задача организации, в особенности же начало повседневной работы по формированию позиции обеспечивает широкому кругу (специалистов) участие в формировании новых предложений. Не следует, однако забывать, что укрепление и развитие структуры требуют определения и уточнения направлений прогрессивного развития. Задача организации, в особенности же реализация намеченных плановых заданий в значительной степени обуславливает создание новых предложений. Разнообразный и богатый опыт консультация с широким активом способствует подготовки и реализации системы обучения кадров, соответствует насущным потребностям. Разнообразный и богатый опыт рамки и место обучения кадров требуют определения и уточнения дальнейших направлений развития.\n\n"
        "Разнообразный и богатый опыт реализация намеченных плановых заданий позволяет оценить значение системы обучения кадров, соответствует насущным потребностям. Товарищи! новая модель организационной деятельности влечет за собой процесс внедрения и модернизации новых предложений. Не следует, однако забывать, что новая модель организационной деятельности требуют от нас анализа дальнейших направлений развития.\n\n"
        "Главным образом консультация с широким активом обеспечивает широкому кругу (специалистов) участие в формировании систем массового участия. С другой стороны новая модель организационной деятельности позволяет выполнять важные задания по разработке соответствующий условий активизации.\n\n"
        "Товарищи! новая модель организационной деятельности представляет собой интересный эксперимент проверки существенных финансовых и административных условий. Идейные соображения высшего порядка, а также сложившаяся структура организации требуют определения и уточнения модели развития. Разнообразный и богатый опыт сложившаяся структура организации требуют от нас анализа модели развития. Разнообразный и богатый опыт начало повседневной работы по формированию позиции играет важную роль в формировании систем массового участия. Разнообразный и богатый опыт реализация намеченных плановых заданий играет важную роль в формировании форм развития. Товарищи! рамки и место обучения кадров в значительной степени обуславливает создание позиций, занимаемых участниками в отношении поставленных задач.\n\n";

static const QString nothing = QString::null;

CStringRef Sampler::getName()
{
    int i = getPoolsValue(namesPool, names.length());
    return names.at(i);
}

CStringRef Sampler::getEngText(ContentMode mode)
{
    switch(mode) {
        default:
        case ContentMode::News: {
            int i = getPoolsValue(textsEngPool, textsEng.length());
            return textsEng.at(i);
        } break;
        case ContentMode::Pangram: {
            return engPangram;
        } break;
        case ContentMode::LoremIpsum: {
            return engLoremIpsum;
        } break;
        case ContentMode::UserDefined: {
            return nothing;
        } break;
    }
}

CStringRef Sampler::getRusText(ContentMode mode)
{
    switch(mode) {
        default:
        case ContentMode::News: {
            int i = getPoolsValue(textsRusPool, textsRus.length());
            return textsRus.at(i);
        } break;
        case ContentMode::Pangram: {
            return rusPangram;
        } break;
        case ContentMode::LoremIpsum: {
            return rusLoremIpsum;
        } break;
        case ContentMode::UserDefined: {
            return nothing;
        } break;
    }
}

void Sampler::loadSample(WorkArea& area)
{
    int i = getPoolsValue(samplesPool, samples.length());
    Sample& sample = samples[i];

    area.addField(InitType::Empty);
    area.addField(InitType::Empty);

    auto &field1 = *area.m_fields[0];
    auto &field2 = *area.m_fields[1];

    area.setSizes({120, 100});

    field1.setFontSize(sample.size1);
    field1.fetchSamples();
    field1.setFontFamily(sample.family1);

    field2.setFontSize(sample.size2);
    field2.fetchSamples();
    field2.setFontFamily(sample.family2);
}

} // namespace fonta
