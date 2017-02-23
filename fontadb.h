#ifndef FONTADB_H
#define FONTADB_H

#include <QFontDatabase>
#include <QString>
#include <QMultiHash>
#include <QSet>
//#include <QHash>
#include <unordered_map>
#include "panose.h"

namespace std
{
    template <>
    struct hash<QString>
    {
        size_t operator()(const QString& s) const
        {
            return qHash(s);
        }
    };
}

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
    Panose panose;
    bool valid;

    // these fields are needed for resolving and control font dependencies while removing font from PC
    // when we delete file we should remove all related files
    // also user should be awared of related fonts which can be removed/reduced while removing particular font
    QSet<QString> files;       // files where this font is defined
    QSet<QString> linkedFonts; // fonts located in the same files with this font

    TTF()
        : familyClass(FamilyClass::NO)
        , familySubClass(0)
        , latin(false)
        , cyrillic(false)
        , panose(0)
        , valid(false)
    {}

    TTF(const TTF &other) = delete;
    TTF &operator= (const TTF &) = delete;

    TTF(TTF &&other) = default;
    TTF &operator= (TTF &&) = default;

    bool isValid() const { return valid; }
    bool isNull() const { return !valid; }
    static const TTF null;
};

struct QtFontInfo {
    bool cyrillic;
    bool monospaced;
    bool symbolic;
};

struct FullFontInfo {
    const TTF *fontaTFF;
    QtFontInfo qtInfo;
    bool TTFExists;
};

class TTFMap : public std::unordered_map<QString, TTF>
{
public:
    bool contains(CStringRef key) const {
        auto res = find(key);
        return res != end();
    }
};

using File2FontsMap = QHash<QString, QSet<QString>>;

class DB : public QObject
{
    Q_OBJECT

private:
    DB();
    DB(const DB &) = delete;
    void operator=(const DB &) = delete;
    static DB *mInstance;

private slots:
    void updateProgress();

signals:
    void emitProgress(int i);
    void loadFinished(int i = 0);

public:
    static DB *instance();
    ~DB();

    void load();

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
    //bool isNotLatinOrCyrillic(CStringRef family) const;

    const TTF &getTTF(CStringRef family) const;
    FullFontInfo getFullFontInfo(CStringRef family) const;

    QFontDatabase& getQtDB() { return *QtDB; }

private:
    QFontDatabase *QtDB;
    TTFMap TTFs;
    File2FontsMap File2Fonts;

    int loadedFiles = 0;
    int filesCount = 0;
    int progress = 0;

    void updateUninstalledFonts();
};

#ifndef FONTA_DETAILED_DEBUG
class LoadThread : public QObject
{
    Q_OBJECT

public:
    LoadThread(const QStringList &out, int from, int to, TTFMap &TTFs, File2FontsMap &File2Fonts)
        : out(out)
        , from(from)
        , to(to)
        , TTFs(TTFs)
        , File2Fonts(File2Fonts)
    {}

public slots:
    void load();

signals:
    void fileLoaded();

private:
    const QStringList &out;
    int from;
    int to;
    TTFMap &TTFs;
    File2FontsMap &File2Fonts;
};
#endif

inline DB& fontaDB() { return *DB::instance(); }
inline QFontDatabase& qtDB() { return fontaDB().getQtDB(); }

} // namespace fonta

#endif // FONTADB_H
