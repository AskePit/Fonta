#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include "types.h"

enum_class (FontType) {
    Start = 0,
    Serif = Start,
    Sans,
    Script,
    Display,
    Symbolic,
    Oldstyle,
    Transitional,
    Modern,
    Slab,
    Grotesque,
    Geometric,
    Humanist,
    Monospaced,
    End

enum_interface
    static QString fileName(FontType::type t) {
        switch(t) {
            case FontType::Serif: return "serif.dat";
            case FontType::Sans: return "sans.dat";
            case FontType::Script: return "script.dat";
            case FontType::Display: return "decorative.dat";
            case FontType::Symbolic: return "symbolic.dat";
            case FontType::Oldstyle: return "old_style.dat";
            case FontType::Transitional: return "transitional.dat";
            case FontType::Modern: return "modern.dat";
            case FontType::Slab: return "slab.dat";
            case FontType::Grotesque: return "grotesque.dat";
            case FontType::Geometric: return "geometric.dat";
            case FontType::Humanist: return "humanist.dat";
            case FontType::Monospaced: return "monospaced.dat";
            default: return "";
        }
    }
};

QString trim(CStringRef name);

#endif // SEARCHENGINE_H
