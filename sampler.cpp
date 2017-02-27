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

    for(const Sample& p : preSamples) {
        if(Family::exists(p.family1) && Family::exists(p.family2)) {
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
    "Before 1960 95% of soft drinks sold in the U.S. are furnished in reusable bottles.",
    "Ernest Hemmingway commits suicide with shotgun.",
    "American U-2 spy plane, piloted by Francis Gary Powers, shot down over Russia",
    "Kennedy was assassinated in Dallas, Texas, on November 22, 1963",
    "Donald Trump promises to dissolve his Trump Foundation charity, which is still under investigation.",
    "Senator Leila de Lima says the charges are an attempt to silence her criticism of the drug war.",
    "Councillors unhappy about absence of sausage stalls at German Earth Day festival.",
    "A pay ruling triggers a passionate backlash about the rights of hospitality and retails workers.",
    "A Canadian hospital will study the effects cannabis oil on a severe form of epilepsy",
    "A distribution company says many of the 251 Freedom handsets it paid for have not been delivered.",
    "Organiser says the ironic event is a protest against fake news.",
    "More fruit and veg might prevent nearly eight million premature deaths each year, researchers say.",
    "The advance marks the first time troops enter an urban district in latest phase of campaign.",
    /*"",
    "",
    "",
    "",
    "",
    "",
    "",*/
};

QStringList Sampler::textsRus = {
    "Шифровальщица попросту забыла ряд ключевых множителей и тэгов",
    "Широкая электрификация южных губерний даст мощный толчок подъёму сельского хозяйства",
    "Подъём с затонувшего эсминца легкобьющейся древнегреческой амфоры сопряжён с техническими трудностями",
    "Лыжник Червоткин завоевал первое золото для сборной России на Военных играх - 2017",
    "Советник президента США опровергла данные о запрете ей давать телеинтервью",
    "Полиция Филиппин арестовала сенатора, критиковавшую президента",
    "Дождь, порывистый ветер и до 5 градусов тепла ожидаются в Московском регионе",
    "\"Прогресс МС-05\" пристыковался к МКС в автоматическом режиме",
    "Рок-музыкант признал свою вину в проносе пистолета на борт самолета",
    /*"",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",*/
};

bool Family::exists(type t)
{
    const QStringList &fontNames = familyMap[t];
    const QStringList &families = fontaDB().families();

    for(CStringRef name : fontNames) {
        if(families.contains(name)) {
            return true;
        }
    }

    return false;
}

QString Family::name(type t)
{
    const QStringList &fontNames = familyMap[t];
    const QStringList &families = fontaDB().families();

    for(CStringRef name : fontNames) {
        if(families.contains(name)) {
            return name;
        }
    }

    return QString::null;
}

const QMap<Family::type, QStringList> Family::familyMap = {
    { Arial, {"Arial"} },
    { ArialBlack, {"Arial Black"} },
    { ArialNarrow, {"Arial Narrow"} },
    { BaskervilleOldFace, {"Baskerville Old Face", "Baskerville Old Face", "Mrs Eaves", "Baskerville" } },
    { BloggerSans, { "Blogger Sans" } },
    { Bodoni, {"Bodoni MT", "Bodoni", "Bodoni Antiqua", "Bodoni Old Face", "ITC Bodoni Seventy Two", "ITC Bodoni Six", "ITC Bodoni Twelve", "LTC Bodoni 175", "WTC Our Bodoni", "Bodoni EF", "Bodoni Classico", "TS Bodoni"} },
    { Calibri, {"Calibri"} },
    { Caslon, {"Centaur", "Adobe Caslon", "Adobe Caslon Pro", "Adobe Caslon Std", "Williams Caslon Text", "LTC Caslon", "Caslon Old Face", "Caslon" } },
    { CenturyGothic, {"Century Gothic"} },
    { CenturySchoolbook, {"Century Schoolbook", "Old Standard"} },
    { Chaparral, {"Chaparral Pro", "Chapparal Std", "Chapparal"} },
    { Clarendon, {"Clarendon"} },
    { Coolvetica, {"Coolvetica", "Coolvetica Rg"} },
    { CooperBlack, {"Cooper Black"} },
    { FranklinGothicBook, {"Franklin Gothic Book"} },
    { FranklinGothicDemi, {"Franklin Gothic Demi"} },
    { FranklinGothicDemiCond, {"Franklin Gothic Demi Cond"} },
    { Futura, {"Futura", "Futura PT", "FuturaLight", "Futura Bk BT"} },
    { Garamond, {"Garamond"} },
    { Georgia, {"Georgia"} },
    { GillSans, {"Gill Sans MT"} },
    { GillSansCondenced, {"Gill Sans MT Condensed"} },
    { Helvetica, {"Helvetica"} },
    { Impact, {"Impact"} },
    { NotoSans, {"Noto Sans"} },
    { Tahoma, {"Tahoma"} },
    { TimesNewRoman, {"Times New Roman"} },
    { Trebuchet, {"Trebuchet MS", "Trebuchet"} },
    { SegoeUI, {"Segoe UI"} },
    { Verdana, {"Verdana"} },
};

const QVector<Sample> Sampler::preSamples = {
    { Family::Georgia, 22, Family::Verdana, 11 },
    { Family::Helvetica, 26, Family::Garamond, 12 },
    { Family::Bodoni, 24, Family::Futura, 16 },
    { Family::Trebuchet, 18, Family::Verdana, 9 },
    { Family::CenturySchoolbook, 21, Family::CenturyGothic, 12 },
    { Family::FranklinGothicDemiCond, 24, Family::CenturyGothic, 12 },
    { Family::Tahoma, 18, Family::SegoeUI, 11 },
    { Family::FranklinGothicDemi, 20, Family::Trebuchet, 12 },
    { Family::ArialBlack, 17, Family::Arial, 11 },
    { Family::Impact, 18, Family::ArialNarrow, 12 },
    { Family::Georgia, 18, Family::Calibri, 12 },
    { Family::SegoeUI, 20, Family::Arial, 11 },
    { Family::Clarendon, 17, Family::TimesNewRoman, 12 },
    { Family::Clarendon, 18, Family::Chaparral, 14 },
    { Family::CooperBlack, 22, Family::Caslon, 17 },
    { Family::Helvetica, 21, Family::Garamond, 15 },
    { Family::Bodoni, 24, Family::GillSans, 14 },
    { Family::Bodoni, 20, Family::GillSans, 14 },
    { Family::GillSans, 20, Family::Caslon, 16 },
    { Family::Caslon, 20, Family::FranklinGothicBook, 12 },
    { Family::Caslon, 20, Family::FranklinGothicBook, 12 },
    { Family::BaskervilleOldFace, 19, Family::FranklinGothicBook, 11 },
    { Family::GillSansCondenced, 20, Family::Arial, 10 },
    { Family::BloggerSans, 20, Family::NotoSans, 10 },
    { Family::Arial, 18, Family::Caslon, 16 },
    { Family::Arial, 18, Family::Calibri, 11 },
    { Family::Coolvetica, 18, Family::ArialNarrow, 12 },
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
    field1.setFontFamily(Family::name(sample.family1));

    field2.setFontSize(sample.size2);
    field2.fetchSamples();
    field2.setFontFamily(Family::name(sample.family2));
}

} // namespace fonta
