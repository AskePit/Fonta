#ifndef TYPES_FONTA_H
#define TYPES_FONTA_H

#include "types.h"
#include <QString>
#include <QApplication>

enum_class (FilterMode) {
    Start,
    ALL = Start,
    CYRILLIC,
    SERIF,
    SANS_SERIF,
    MONOSPACE,
    SCRIPT,
    DECORATIVE,
    SYMBOLIC,
    End,
    CUSTOM = End

    enum_interface
    static QString toString(type t) {
        switch(t) {
        default:
        case ALL:           return QApplication::translate("FilterMode", "[All]");         break;
        case CYRILLIC:      return QApplication::translate("FilterMode", "Cyrillic");      break;
        case SERIF:         return QApplication::translate("FilterMode", "Serif");         break;
        case SANS_SERIF:    return QApplication::translate("FilterMode", "Sans Serif");    break;
        case MONOSPACE:     return QApplication::translate("FilterMode", "Monospace");     break;
        case SCRIPT:        return QApplication::translate("FilterMode", "Script");        break;
        case DECORATIVE:    return QApplication::translate("FilterMode", "Decorative");    break;
        case SYMBOLIC:      return QApplication::translate("FilterMode", "Symbolic");      break;
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
