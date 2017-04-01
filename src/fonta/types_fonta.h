#ifndef TYPES_FONTA_H
#define TYPES_FONTA_H

#include "types.h"
#include <QString>

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
        case ALL:           return tr("[All]");         break;
        case CYRILLIC:      return tr("Cyrillic");      break;
        case SERIF:         return tr("Serif");         break;
        case SANS_SERIF:    return tr("Sans Serif");    break;
        case MONOSPACE:     return tr("Monospace");     break;
        case SCRIPT:        return tr("Script");        break;
        case DECORATIVE:    return tr("Decorative");    break;
        case SYMBOLIC:      return tr("Symbolic");      break;
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
