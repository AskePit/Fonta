#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include "types.h"

namespace fonta {

enum_class (FontType) {
    Serif        = (1<<0),
    Sans         = (1<<1),
    Script       = (1<<2),
    Display      = (1<<3),
    Symbolic     = (1<<4),
    Oldstyle     = (1<<5),
    Transitional = (1<<6),
    Modern       = (1<<7),
    Slab         = (1<<8),
    Grotesque    = (1<<9),
    Geometric    = (1<<10),
    Humanist     = (1<<11),
    Monospaced   = (1<<12),

enum_interface
    static QString fileName(FontType::type t) {
        switch(t) {
            case Serif: return "serif.dat";
            case Sans: return "sans.dat";
            case Script: return "script.dat";
            case Display: return "decorative.dat";
            case Symbolic: return "symbolic.dat";
            case Oldstyle: return "old_style.dat";
            case Transitional: return "transitional.dat";
            case Modern: return "modern.dat";
            case Slab: return "slab.dat";
            case Grotesque: return "grotesque.dat";
            case Geometric: return "geometric.dat";
            case Humanist: return "humanist.dat";
            case Monospaced: return "monospaced.dat";
            default: return "";
        }
    }

    static QString name(FontType::type t) {
        switch(t) {
            case Serif: return "Serif";
            case Sans: return "Sans";
            case Script: return "Script";
            case Display: return "Decorative";
            case Symbolic: return "Symbolic";
            case Oldstyle: return "Old Style";
            case Transitional: return "Transitional";
            case Modern: return "Modern";
            case Slab: return "Slab";
            case Grotesque: return "Grotesque";
            case Geometric: return "Geometric";
            case Humanist: return "Humanist";
            case Monospaced: return "Monospaced";
            default: return "";
        }
    }

    static const QVector<FontType::type> &enumerate() {
        static const QVector<FontType::type> vec = {
            Serif, Sans, Script, Display, Symbolic, Oldstyle, Transitional,
            Modern, Slab, Grotesque, Geometric, Humanist, Monospaced,
        };
        return vec;
    }

    static bool exists(int val) {
        return val != 0;
    }

    static bool isSerif(int val) {
        return val & (Serif | Oldstyle | Transitional | Modern | Slab);
    }

    static bool isSans(int val) {
        return val & (Sans | Grotesque | Geometric | Humanist);
    }
};

QString trim(CStringRef name);

class Classifier
{
public:

    bool load(CStringRef dbPath);
    void store();

    int fontInfo(CStringRef family) const;
    void addFontInfo(CStringRef family, int info);
    void rewriteFontInfo(CStringRef family, int info);

private:
    QString m_dbPath;
    QMap<FontType::type, QStringList> m_db;
    bool m_changed {false};

    bool loadFontType(FontType::type tupe);
    bool storeFontType(FontType::type type);
    void _addFontInfo(CStringRef family, int info);
};


} // namespace fonta

#endif // SEARCHENGINE_H
