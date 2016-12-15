#include "sampler.h"

#include <cstdlib>
#include <QVector>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QXmlStreamReader>
#include <QEventLoop>
#include <QObject>

#ifdef FONTA_MEASURES
#include <QElapsedTimer>
#include <QDebug>
#endif

#include "fontadb.h"

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
};

QStringList Sampler::texts = {
    "The quick brown fox jumps over the lazy dog",
    "Pack my box with five dozen liquor jugs",
    "Jackdaws love my big sphinx of quartz",
    "Cozy lummox gives smart squid who asks for job pen",
    "Wafting zephyrs quickly vexed jumbo",
    "Waltz nymph for quick jigs vex Bud",
    "The five boxing wizards jump quickly",
    "Mix Zapf with Veljovic and get quirky Beziers",
    "Queen Elizabeth's proxy waved off Mick Jagger",
    "Quick jigs for waltz vex bad nymph",
    "Sphinx of black quartz, judge my vow",
    "Then a cop quizzed Mick Jagger's ex-wives briefly",
    "Sexy diva Jeniffer Lopez wasn't baking me quiche",
    "Five big quacking zephyrs jolt my wax bed",
    "The big plump jowls of zany Dick Nixon quiver",
    "Mix Zapf with Veljovic and get quircky Beziers",
    "Quickly Shortz dreams up a few vexing jumbles",
    "Blowzy red vixens fight for a quick jump",
    "Turgid saxophones blew over Mick's jazzy quiff",
    "Quincy Jones vowed to fix the bleak jazz program",
    "Quick zephyrs blow, vexing daft Jim",
    "Brick quiz whangs jumpy veldt fox!",
    "Mr. Jock, TV quiz Ph.D., bags few lynx.",
    "Dumpy kibitzer jingles as exchequer overflows",
    "Puzzled women bequeath jerks very exotic gifts",
    "Putin backs away from Iraq zeal; Bush, vexed, jogs.",
    "Franz jagt im komplett verwahrlosten Taxi quer durch Bayern",
};

QStringList Sampler::textsRus = {
    "Съешь же ещё этих мягких французских булок да выпей чаю",
    "Аэрофотосъёмка ландшафта уже выявила земли богачей и процветающих крестьян",
    "Южно-эфиопский грач увёл мышь за хобот на съезд ящериц",
    "Шифровальщица попросту забыла ряд ключевых множителей и тэгов",
    "Безмозглый широковещательный цифровой передатчик сужающихся экспонент",
    "Однажды съев фейхоа, я, как зацикленный, ностальгирую всё чаще и больше по этому чуду",
    "Блеф разъедает ум, чаще цыгана живёшь беспокойно",
    "Пиши: зять съел яйцо, ещё чан брюквы...",
    "Флегматичная эта верблюдица жует у подъезда засыхающий горький шиповник",
    "Вступив в бой с шипящими змеями — эфой и гадюкой, — маленький, цепкий, храбрый ёж съел их",
    "Широкая электрификация южных губерний даст мощный толчок подъёму сельского хозяйства",
    "Подъехал шофёр на рефрижераторе грузить яйца для обучающихся элитных медиков",
    "Подъём с затонувшего эсминца легкобьющейся древнегреческой амфоры сопряжён с техническими трудностями",
    "Мюзикл-буфф «Огнедышащий простужается ночью» (в 12345 сценах и 67890 эпизодах)",
    "Буквопечатающей связи нужны хорошие э/магнитные реле. Дать цифры (1234567890+= .?-)",
};

const QVector<Sample> Sampler::preSamples = {
    {
        "Georgia", 24,
        "Verdana", 9
    },
    {
        "Helvetica", 26,
        "Garamond", 12
    },
    {
        "Bodoni MT", 28,
        "FuturaLight", 14
    },
    {
        "Trebuchet MS", 18,
        "Verdana", 9
    },
    {
        "Calisto MT", 28,
        "Century Gothic", 11
    },
    {
        "Century Schoolbook", 30,
        "Century Gothic", 11
    },
    {
        "Franklin Gothic Demi Cond", 32,
        "Century Gothic", 11
    },
    {
        "Tahoma", 18,
        "Segoe UI", 11
    },
    {
        "Impact", 24,
        "Trebuchet MS", 12
    },
    {
        "Trebuchet MS", 26,
        "Corbel", 13
    },
    {
        "Arial Black", 24,
        "Arial", 11
    },
    {
        "Impact", 26,
        "Arial Narrow", 12
    },
    {
        "Cambria", 20,
        "Calibri", 11
    },
    {
        "Segoe UI", 26,
        "Arial", 11
    },
    {
        "Terminal", 14,
        "Terminal", 12
    },
    {
        "Clarendon", 24,
        "Times New Roman", 12
    },
    {
        "Century Gothic", 20,
        "Arial Narrow", 12
    },
};

void fetchNews(QStringList &list, const QString &url, const QString &tag)
{
    QNetworkAccessManager manager;
    QNetworkReply *reply = manager.get(QNetworkRequest(url));

    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));

#ifdef FONTA_MEASURES
    QElapsedTimer timer;
    timer.start();
#endif

    loop.exec();
    if(reply->error() != QNetworkReply::NoError) {
        return;
    }

#ifdef FONTA_MEASURES
    qDebug() << timer.elapsed() << "ms to load news";
    timer.start();
#endif

    list.clear();
    QXmlStreamReader r(reply->readAll());
    while(r.readNextStartElement());
    while(!r.atEnd()) {
        r.readNext();
        if(r.name() == "item") {
            while(!r.atEnd()) {
                r.readNext();
                if(r.name() == tag) {
                    list << r.readElementText();
                    break;
                }
            }
        }
    }

#ifdef FONTA_MEASURES
    qDebug() << timer.elapsed() << "ms to process news rss-xml";
#endif
}

void Sampler::initSamples()
{
    // try to fetch rss news
    fetchNews(texts, "http://feeds.bbci.co.uk/news/world/rss.xml", "description");
    fetchNews(textsRus, "http://tass.ru/rss/v2.xml", "title");

    // filter font pair depending on users installed fonts
    const QStringList& families = fontaDB().families();

    for(const Sample& p : preSamples) {
        if(families.contains(p.family1) && families.contains(p.family2)) {
            samples << p;
        }
    }
}

QVector<Sample> Sampler::samples;
QSet<int> Sampler::namesPool;
QSet<int> Sampler::textsPool;
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

CStringRef Sampler::getName()
{
    int i = getPoolsValue(namesPool, names.length());
    return names.at(i);
}

CStringRef Sampler::getText()
{
    int i = getPoolsValue(textsPool, texts.length());
    return texts.at(i);
}

CStringRef Sampler::getRusText()
{
    int i = getPoolsValue(textsRusPool, textsRus.length());
    return textsRus.at(i);
}

CStringRef Sampler::getTextForFamily(CStringRef family)
{
    if(fontaDB().isCyrillic(family)) {
        return getRusText();
    } else {
        return getText();
    }
}

void Sampler::loadSample(FontaWorkArea& area)
{
    int i = getPoolsValue(samplesPool, samples.length());
    Sample& sample = samples[i];

    area.addField(true);
    area.addField(true);

    auto &field1 = *area.m_fields[0];
    auto &field2 = *area.m_fields[1];

    area.setSizes({120, 100});

    field1.setFontSize(sample.size1);
    field1.setSamples(getText(), getRusText());
    field1.setFontFamily(sample.family1);

    field2.setFontSize(sample.size2);
    field2.setSamples(getText(), getRusText());
    field2.setFontFamily(sample.family2);
}
