#ifndef TYPES_FONTA_H
#define TYPES_FONTA_H

#include "types.h"
#include <QString>
#include <QApplication>

enum_class (FilterMode) {
    Start,
    All = Start,
    Cyrillic,
    Serif,
    SansSerif,
    Monospace,
    Script,
    Decorative,
    Symbolic,
    End,
    Custom = End

    enum_interface
    static QString toString(type t) {
        switch(t) {
        default:
        case All:           return QApplication::translate("FilterMode", "[All]");         break;
        case Cyrillic:      return QApplication::translate("FilterMode", "Cyrillic");      break;
        case Serif:         return QApplication::translate("FilterMode", "Serif");         break;
        case SansSerif:     return QApplication::translate("FilterMode", "Sans Serif");    break;
        case Monospace:     return QApplication::translate("FilterMode", "Monospace");     break;
        case Script:        return QApplication::translate("FilterMode", "Script");        break;
        case Decorative:    return QApplication::translate("FilterMode", "Decorative");    break;
        case Symbolic:      return QApplication::translate("FilterMode", "Symbolic");      break;
        }
    }
};

enum class InitType {
    Empty,
    Sampled
};

enum class ContentMode {
    News,
    Pangram,
    LoremIpsum,
    UserDefined
};

enum class LanguageContext {
    Auto,
    Eng,
    Rus,
    UserDefined
};

#endif // TYPES_FONTA_H
