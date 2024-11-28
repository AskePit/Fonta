#include "classifier.h"

#include <QStringList>
#include <QStringView>
#include <QVector>
#include <QDir>
#include <QDebug>

namespace fonta {

static const QStringList prefixes = {
	QStringLiteral("Adobe"),
	QStringLiteral("AR"),
	QStringLiteral("AG"),
	QStringLiteral("ATC"),
	QStringLiteral("ITC"),
	QStringLiteral("Libra"),
	QStringLiteral("Libre"),
	QStringLiteral("Microsoft"),
	QStringLiteral("PF"),
	QStringLiteral("TT"),
	QStringLiteral("v"),
};

static const QStringList postfixes = {
	QStringLiteral("Pro"),
	QStringLiteral("Std"),
	QStringLiteral("Standard"),
	QStringLiteral("Regular"),
	QStringLiteral("Normal"),
	QStringLiteral("Medium"),
	QStringLiteral("R"),
	QStringLiteral("Rg"),
	QStringLiteral("Reg"),
	QStringLiteral("Md"),
	QStringLiteral("Med"),
	QStringLiteral("Black"),
	QStringLiteral("Heavy"),
	QStringLiteral("Bk"),
	QStringLiteral("Blk"),
	QStringLiteral("Hv"),
	QStringLiteral("Bold"),
	QStringLiteral("B"),
	QStringLiteral("Bd"),
	QStringLiteral("Semibold"),
	QStringLiteral("Demibold"),
	QStringLiteral("Smbd"),
	QStringLiteral("SeBd"),
	QStringLiteral("DmBd"),
	QStringLiteral("Thin"),
	QStringLiteral("XThin"),
	QStringLiteral("Light"),
	QStringLiteral("Lite"),
	QStringLiteral("Semilight"),
	QStringLiteral("L"),
	QStringLiteral("LT"),
	QStringLiteral("Lt"),
	QStringLiteral("Italic"),
	QStringLiteral("Oblique"),
	QStringLiteral("Rounded"),
	QStringLiteral("Hair"),
	QStringLiteral("Sketch"),
	QStringLiteral("Poster"),
	QStringLiteral("Compressed"),
	QStringLiteral("Condensed"),
	QStringLiteral("Cond"),
	QStringLiteral("Dsp"),
	QStringLiteral("Cn"),
	QStringLiteral("Narrow"),
	QStringLiteral("Demo"),
	QStringLiteral("Extended"),
	QStringLiteral("Tit"),
	QStringLiteral("Txt"),
	QStringLiteral("Ext"),
	QStringLiteral("Text"),
	QStringLiteral("Caption"),
	QStringLiteral("Display"),
	QStringLiteral("Subhead"),
	QStringLiteral("SmText"),
	QStringLiteral("FF"),
	QStringLiteral("Demi"),
	QStringLiteral("Semi"),
	QStringLiteral("Extra"),
	QStringLiteral("Ultra"),
	QStringLiteral("Personal"),
	QStringLiteral("Cyr"),
	QStringLiteral("Cyrl"),
	QStringLiteral("CYR"),
	QStringLiteral("SC"),
	QStringLiteral("BT"),
	QStringLiteral("MT"),
	QStringLiteral("MS"),
	QStringLiteral("ITC"),
	QStringLiteral("Armenian"),
	QStringLiteral("Devanagari"),
	QStringLiteral("Ethiopic"),
	QStringLiteral("Georgian"),
	QStringLiteral("Hebrew"),
	QStringLiteral("Heiti"),
	QStringLiteral("Khmer"),
	QStringLiteral("Lao"),
	QStringLiteral("Tamil"),
	QStringLiteral("Thai"),
	QStringLiteral("Khmer"),
};

static const QStringList sansHints = {
    QStringLiteral(" sans serif"),
    QStringLiteral(" sans"),
};

static const QStringList serifHints = {
    " serif",
};

static const QStringList scriptHints = {
    QStringLiteral(" script"),
    QStringLiteral(" brush"),
    QStringLiteral(" pen"),
    QStringLiteral(" marker"),
};

static const QStringList modernHints = {
    QStringLiteral(" didone"),
};

static const QStringList slabHints = {
    QStringLiteral(" slab"),
};

static const QStringList monospacedHints = {
    QStringLiteral(" mono"),
};

static QVector<QStringView> split(CStringRef str) {
    QStringView s(str);

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

    QVector<QStringView> res;
    last = indexes.length()-1;
    for(int i = 0; i<last+1; ++i) {
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

bool Classifier::load(CStringRef dbPath)
{
    m_db.clear();
    m_dbPath = dbPath;

    for(cauto type : FontType::enumerate()) {
        bool ok = loadFontType(type);
        if(!ok) {
            return false;
        }
    }

    return true;
}

bool Classifier::loadFontType(FontType::type type)
{
    QFile file(m_dbPath + QDir::separator() + FontType::fileName(type));

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream textStream(&file);
    QStringList &list = m_db[type];
    while (!textStream.atEnd()) {
        list << textStream.readLine();
    }

    file.close();

    list.removeDuplicates();
    list.removeOne(QStringLiteral(""));

    return true;
}

void Classifier::store()
{
    if(!m_changed) {
        qDebug() << QCoreApplication::translate("fonta::Classifier", "Did not changed. Do nothing");
        return;
    }

    qDebug() << QCoreApplication::translate("fonta::Classifier", "Changed. Store");

    for(cauto type : FontType::enumerate()) {
        storeFontType(type);
    }
}

bool Classifier::storeFontType(FontType::type type)
{
    QFile file(m_dbPath + QDir::separator() + FontType::fileName(type));

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }

    m_db[type].sort(Qt::CaseInsensitive);
    m_db[type].removeDuplicates();

    QTextStream textStream(&file);
    for(CStringRef str : std::as_const(m_db[type])) {
        textStream << str << QLatin1String("\n");
    }

    file.close();
    return true;
}

int Classifier::fontInfo(CStringRef family, SearchType searchType) const
{
    int info = 0;

    QString trimmed = family.trimmed();

    for(cauto type : FontType::enumerate()) {
        if(m_db[type].contains(trimmed)) {
            info |= type;
        }

    }

    if(!FontType::exists(info) && searchType == AdvancedSearch) {
        static const QVector<QPair<FontType::type, const QStringList &>> hintsMap = {
            { FontType::Sans, sansHints },
            { FontType::Serif, serifHints },
            { FontType::Script, scriptHints },
            { FontType::Modern, modernHints },
            { FontType::Slab, slabHints },
            { FontType::Monospaced, monospacedHints },
        };

        for(cauto pair : hintsMap) {
            for(CStringRef hint : std::as_const(pair.second)) {
                if(trimmed.contains(hint)) {
                    info |= pair.first;
                    break;
                }
            }
        }
    }

    return normalizeInfo(info);
}

void Classifier::_addFontInfo(CStringRef family, int info)
{
    info = internalNormalizeInfo(info);

    for(cauto type : FontType::enumerate()) {
        if(info & type) {
            if(!m_db[type].contains(family)) {
                qDebug() << family << QStringLiteral("added");
                m_db[type] << family;
            }
        }
    }

    m_changed = true;
}

void Classifier::addFontInfo(CStringRef family, int info)
{
    if(!FontType::exists(info)) {
        return;
    }

    QString trimmed = family.trimmed();

    _addFontInfo(trimmed, info);
}

void Classifier::rewriteFontInfo(CStringRef family, int info)
{
    if(!FontType::exists(info)) {
        return;
    }

    QString trimmed = family.trimmed();

    for(cauto type : FontType::enumerate()) {
        m_db[type].removeOne(trimmed);
    }

    _addFontInfo(trimmed, info);
}

int Classifier::normalizeInfo(int info)
{
    if((info & FontType::Sans) && (info & FontType::Serif)) {
        info &= ~FontType::Serif;
    }

    if((info & FontType::Oldstyle)
    || (info & FontType::Transitional)
    || (info & FontType::Modern)
    || (info & FontType::Slab)) {
        info |= FontType::Serif;
    }

    if((info & FontType::Grotesque)
    || (info & FontType::Geometric)
    || (info & FontType::Humanist)) {
        info |= FontType::Sans;
    }

    return info;
}

int Classifier::internalNormalizeInfo(int info)
{
    if((info & FontType::Sans) && (info & FontType::Serif)) {
        info &= ~FontType::Serif;
    }

    if((info & FontType::Oldstyle)
    || (info & FontType::Transitional)
    || (info & FontType::Modern)
    || (info & FontType::Slab)) {
        info &= ~FontType::Serif;
    }

    if((info & FontType::Grotesque)
    || (info & FontType::Geometric)
    || (info & FontType::Humanist)) {
        info &= ~FontType::Sans;
    }

    return info;
}


} // namespace fonta
