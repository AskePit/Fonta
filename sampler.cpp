#include "sampler.h"

#include <cstdlib>
#include <QVector>
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

const QStringList Sampler::texts = {
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
};

const QStringList Sampler::textsRus = {
    "Съешь же ещё этих мягких французских булок да выпей чаю",
    "В чащах юга жил бы цитрус? Да, но фальшивый экземпляр!",
    "Эй, жлоб! Где туз? Прячь юных съёмщиц в шкаф",
    "Чушь: гид вёз кэб цапф, юный жмот съел хрящ",
    "Южно-эфиопский грач увёл мышь за хобот на съезд ящериц",
    "Шифровальщица попросту забыла ряд ключевых множителей и тэгов",
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
        "Trebuchet MS", 24,
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
        "Trebuchet MS", 14
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
};

void Sampler::initSamples()
{
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

    area.addField();
    area.addField();

    auto &field1 = *area.m_fields[0];
    auto &field2 = *area.m_fields[1];

    area.setSizes(QList<int>() << 120 << 100);

    field1.setPreferableFontStyle("Normal");
    field1.setFontSize(sample.size1);
    field1.setSamples(getText(), getRusText());
    field1.setFontFamily(sample.family1);

    field2.setPreferableFontStyle("Normal");
    field2.setFontSize(sample.size2);
    field2.setSamples(getText(), getRusText());
    field2.setFontFamily(sample.family2);
}
