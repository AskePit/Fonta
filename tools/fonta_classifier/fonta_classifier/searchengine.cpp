#include "searchengine.h"

#include <QStringList>
#include <QVector>
#include <QDebug>

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
    "Sketch",

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
    "FF",

    "Demi",
    "Semi",
    "Extra",
    "Ultra",

    "Personal",

    "Cyr",
    "Cyrl",
    "CYR",

    "SC",

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

static QStringList modernHints = {
    "Didone",
};

static QStringList slabHints = {
    "Slab",
};

static QStringList monospacedHints = {
    "Mono",
};

static QVector<QStringRef> split(CStringRef str) {
    QStringRef s(&str);

    QVector<int> indexes{0};
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

    QVector<QStringRef> res;
    last = indexes.length()-1;
    for(int i = 0; i<indexes.length(); ++i) {
        int pos = indexes[i];
        int l = (i != last) ? indexes[i+1] - pos : -1;
        res.push_back(s.mid(pos, l));
    }

    return res;
}


//! Constructs QString from QStringRef without deep copy.
//! Be sure that owner of data is valid during all the retuned QString lifetime!!
#define getSharedQString(ref) QString::fromRawData(ref.unicode(), ref.count())

//! Force a deep copy of string. Usually used for QStrings created by getSharedQString function
#define getCopiedQString(str) QString::fromUtf16(str.utf16())

QString trim(CStringRef name)
{
    QString str = name.simplified();
    str.remove('-');
    str.remove('_');

    QVector<QStringRef> v = str.contains(' ') ? QStringRef(&str).split(' ', QString::SkipEmptyParts) : split(str);

    while(v.length() > 1 && prefixes.contains(getSharedQString(v[0]))) {
        v.pop_front();
    }

    for(int i = 1; i<v.length(); ++i) {
        QString fromRef(getSharedQString(v[i]));
        if(postfixes.contains(fromRef, Qt::CaseInsensitive)) {
            v.remove(i, v.length()-i);
        }
    }

    QString res;
    for(cauto ref : v) {
        res += getSharedQString(ref) + ' ';
    }
    res.truncate(res.length()-1);

    return res;
}
