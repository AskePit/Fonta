#include "searchengine.h"

#include <QStringList>
#include <QVector>

/**
 * Font trim:
 *
 * trim, simplify, remove spaces and _
 * remove prefixes
 * split on words
 * IF several words
 *     look for first postfix, delete it and all to the right
 */

static QStringList prefixes = {
    "Adobe",
    "AR",
    "AG",
    "ATC",
    "Microsoft",
    "PF",
    "TT",
    "v",
};

static QStringList postfixes = {
    "Pro",
    "Std",
    "Standard",

    "Regular",
    "Normal",
    "Medium",
    "R",
    "Rg",
    "Reg",
    "Md",
    "Med",

    "Black",
    "Heavy",
    "Bk",
    "Blk",
    "Hv",

    "Bold",
    "B",
    "Bd",
    "Semibold",
    "Demibold",
    "Smbd",
    "SeBd",
    "DmBd",

    "Thin",
    "XThin",

    "Light",
    "Lite",
    "Semilight",
    "L",
    "LT",
    "Lt",

    "Italic",
    "Oblique",
    "Rounded",
    "Hair",

    "Poster",
    "Compressed",
    "Condensed",
    "Cond",
    "Dsp",
    "Cn",
    "Narrow",
    "Demo",
    "Extended",
    "Tit",
    "Txt",
    "Ext",
    "Text",
    "Caption",
    "Display",
    "Subhead",
    "SmText",

    "Demi",
    "Semi",
    "Extra",
    "Ultra",

    "Personal",
    "Use",
    "Only",

    "Cyr",
    "Cyrl",
    "CYR",

    "BT",
    "MT",
    "MS",
    "ITC",

    "Armenian",
    "Devanagari",
    "Ethiopic",
    "Georgian",
    "Hebrew",
    "Heiti",
    "Khmer",
    "Lao",
    "Tamil",
    "Thai",
    "Khmer",
};

static QStringList sansHints = {
    "SansSerif",
    "Sans",
};

static QStringList serifHints = {
    "Serif",
};

static QStringList scriptHints = {
    "Script",
    "Brush",
    "Pen",
    "Marker",
};

static QStringList monospacedHints = {
    "Mono",
};

static QVector<QStringRef> split(CStringRef str) {
    QStringRef s(&str);

    QList<int> indexes {0};
    int last = s.length()-1;
    for(int i = 1; i<s.length(); ++i) {
        const QChar curr = s.at(i);
        if(!curr.isUpper()) {
            continue;
        }

        const QChar prev = s.at(i-1);
        const QChar next = (i != last) ? s.at(i+1) : 'A'; // just any capital

        if(next.isLower() || prev.isLower()) {
            indexes.push_back(i);
        }
    }


}

QString easy_trim(CStringRef name)
{
    QString res(name);
    res.remove(' ');
    res.remove('_');
    res.remove('-');

    return res;
}

QString trim(CStringRef name)
{
    QString res = name.simplified();
    res.remove(' ');
    res.remove('_');
    res.remove('-');

    return res;
}
