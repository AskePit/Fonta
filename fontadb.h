#ifndef FONTADB_H
#define FONTADB_H

#include <QFontDatabase>
#include <QString>
#include <QMultiHash>
#include <QSet>
#include <QHash>
#include "panose.h"

namespace fonta {

enum_class (FamilyClass) {
    NO = 0,
    OLDSTYLE_SERIF = 1,
    TRANSITIONAL_SERIF = 2,
    MODERN_SERIF = 3,
    CLARENDON_SERIF = 4,
    SLAB_SERIF = 5,
    FREEFORM_SERIF = 7,
    SANS_SERIF = 8,
    ORNAMENTAL = 9,
    SCRIPT = 10,
    SYMBOL = 12,

enum_interface

    static QString toString(type t) {
        switch(t) {
            default:
            case NO:                 return "No Info";            break;
            case OLDSTYLE_SERIF:     return "Oldstyle Serif";     break;
            case TRANSITIONAL_SERIF: return "Transitional Serif"; break;
            case MODERN_SERIF:       return "Modern Serif";       break;
            case CLARENDON_SERIF:    return "Clarendon Serif";     break;
            case SLAB_SERIF:         return "Slab Serif";         break;
            case FREEFORM_SERIF:     return "Freeform Serif";      break;
            case SANS_SERIF:         return "Sans Serif";         break;
            case ORNAMENTAL:         return "Decorative";         break;
            case SCRIPT:             return "Script";             break;
            case SYMBOL:             return "Symbolic";           break;
        }
    }

    static bool noInfo(type t) { return t == NO || t == 6 || t == 11 || t == 13 || t == 14; }
    static bool isSerif(type t) { return (t >= OLDSTYLE_SERIF && t <= SLAB_SERIF) || (t == FREEFORM_SERIF); }
    static bool isSans(type t) { return t == SANS_SERIF; }
};

struct TTF {
    FamilyClass::type familyClass;
    int familySubClass;
    bool latin;
    bool cyrillic;
    bool monospaced;
    Panose panose;

    // these fields are needed for resolving and control font dependencies while removing font from PC
    // when we delete file we should remove all related files
    // also user should be awared of related fonts which can be removed/reduced while removing particular font
    QSet<QString> files;       // files where this font is defined
    QSet<QString> linkedFonts; // fonts located in the same files with this font

    TTF()
        : familyClass(FamilyClass::NO)
        , latin(false)
        , cyrillic(false)
        , monospaced(false)
        , panose(0)
    {}
};

struct QtFontInfo {
    bool cyrillic;
    bool monospaced;
    bool symbolic;
};

struct FullFontInfo {
    TTF fontaTFF;
    QtFontInfo qtInfo;
    bool TTFExists;
};

using TTFMap = QHash<QString, TTF>;
using File2FontsMap = QHash<QString, QSet<QString>>;

class DB
{
private:
    DB();
    DB(DB const&);
    void operator=(DB const&);

public:
    static DB &getInstance()
    {
        static DB instance;
        return instance;
    }
    ~DB();

    QStringList families() const;
    QStringList styles(CStringRef family) const { return QtDB->styles(family); }
    QFont font(CStringRef family, CStringRef style, int pointSize) const { return QtDB->font(family, style, pointSize); }
    QStringList linkedFonts(CStringRef family) const;
    QStringList fontFiles(CStringRef family) const;

    void uninstall(CStringRef family);
    QStringList uninstalled() const;
    QStringList filesToDelete() const;

    bool isAnyFont(CStringRef family) const { (void)family; return true; }
    bool isSerif(CStringRef family) const;
    bool isSansSerif(CStringRef family) const;
    bool isMonospaced(CStringRef family) const;
    bool isScript(CStringRef family) const;
    bool isDecorative(CStringRef family) const;
    bool isSymbolic(CStringRef family) const;

    bool isOldStyle(CStringRef family) const;
    bool isTransitional(CStringRef family) const;
    bool isModern(CStringRef family) const;
    bool isSlab(CStringRef family) const;

    bool isCoveSerif(CStringRef family) const;
    bool isSquareSerif(CStringRef family) const;
    bool isBoneSerif(CStringRef family) const;
    bool isAsymmetricSerif(CStringRef family) const;
    bool isTriangleSerif(CStringRef family) const;

    bool isGrotesque(CStringRef family) const;
    bool isGeometric(CStringRef family) const;
    bool isHumanist(CStringRef family) const;

    bool isNormalSans(CStringRef family) const;
    bool isRoundedSans(CStringRef family) const;
    bool isFlarredSans(CStringRef family) const;

    bool isNonCyrillic(CStringRef family) const;
    bool isCyrillic(CStringRef family) const;
    bool isNotLatinOrCyrillic(CStringRef family) const;

    bool getTTF(CStringRef family, TTF& ttf) const;
    FullFontInfo getFullFontInfo(CStringRef family) const;

    QFontDatabase& getQtDB() { return *QtDB; }

private:
    QFontDatabase *QtDB;
    TTFMap TTFs;
    File2FontsMap File2Fonts;
};

inline DB& fontaDB() { return DB::getInstance(); }
inline QFontDatabase& qtDB() { return fontaDB().getQtDB(); }

}

#endif // FONTADB_H
