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
    QStringList styles(const QString& family) const { return QtDB.styles(family); }
    QFont font(const QString& family, const QString& style, int pointSize) const { return QtDB.font(family, style, pointSize); }

    bool isAnyFont(const QString& family) const { (void)family; return true; }
    bool isCyrillic(const QString& family) const;
    bool isSerif(const QString& family) const;
    bool isSansSerif(const QString& family) const;
    bool isMonospaced(const QString& family) const;
    bool isScript(const QString& family) const;
    bool isDecorative(const QString& family) const;
    bool isSymbolic(const QString& family) const;

    bool isOldStyle(const QString& family) const;
    bool isTransitional(const QString& family) const;
    bool isModern(const QString& family) const;
    bool isSlab(const QString& family) const;

    bool isCoveSerif(const QString& family) const;
    bool isSquareSerif(const QString& family) const;
    bool isBoneSerif(const QString& family) const;
    bool isAsymmetricSerif(const QString& family) const;
    bool isTriangleSerif(const QString& family) const;

    bool isGrotesque(const QString& family) const;
    bool isGeometric(const QString& family) const;
    bool isHumanist(const QString& family) const;

    bool isNormalSans(const QString& family) const;
    bool isRoundedSans(const QString& family) const;
    bool isFlarredSans(const QString& family) const;

    bool getTTF(const QString& family, FontaTTF& ttf) const;
    FullFontInfo getFullFontInfo(const QString& family) const;

    QFontDatabase& getQtDB() { return QtDB; }

private:
    QFontDatabase QtDB;
    QHash<QString, FontaTTF> TTFs;
};

inline FontaDB& fontaDB() { return FontaDB::getInstance(); }
inline QFontDatabase& qtDB() { return fontaDB().getQtDB(); }

#endif // FONTADB_H
