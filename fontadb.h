#ifndef FONTADB_H
#define FONTADB_H

#include <QFontDatabase>
#include <QString>
#include <QHash>
#include "panose.h"

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

struct FontaTTF {
    FamilyClass::type familyClass;
    int familySubClass;
    bool cyrillic;
    bool monospaced;
    Panose panose;
    QStringList files;

    FontaTTF()
        : familyClass(FamilyClass::NO)
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
    FontaTTF fontaTFF;
    QtFontInfo qtInfo;
    bool TTFExists;
};

class FontaDB
{
private:
    FontaDB();
    FontaDB(FontaDB const&);
    void operator=(FontaDB const&);

public:
    static FontaDB& getInstance()
    {
        static FontaDB instance;
        return instance;
    }
    ~FontaDB();

    QStringList families() const { return QtDB.families(); }
    QStringList styles(CStringRef family) const { return QtDB.styles(family); }
    QFont font(CStringRef family, CStringRef style, int pointSize) const { return QtDB.font(family, style, pointSize); }

    bool isAnyFont(CStringRef family) const { (void)family; return true; }
    bool isCyrillic(CStringRef family) const;
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

    bool getTTF(CStringRef family, FontaTTF& ttf) const;
    FullFontInfo getFullFontInfo(CStringRef family) const;

    QFontDatabase& getQtDB() { return QtDB; }

private:
    QFontDatabase QtDB;
    QHash<QString, FontaTTF> TTFs;
};

inline FontaDB& fontaDB() { return FontaDB::getInstance(); }
inline QFontDatabase& qtDB() { return fontaDB().getQtDB(); }

#endif // FONTADB_H
