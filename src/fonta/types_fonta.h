#ifndef TYPES_FONTA_H
#define TYPES_FONTA_H

#include "types.h"
#include <QString>

struct Version {
    int major;
    int minor;
    int build;
    QString str;

    Version(int major, int minor, int build)
        : major(major)
        , minor(minor)
        , build(build)
    {
        const QChar dot = '.';
        str = QString::number(major);
        str += dot;
        str += QString::number(minor);
        str += dot;
        str += QString::number(build);
    }

    ~Version() {}
};

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
        case ALL:           return "[All]";         break;
        case CYRILLIC:      return "Cyrillic";      break;
        case SERIF:         return "Serif";         break;
        case SANS_SERIF:    return "Sans Serif";    break;
        case MONOSPACE:     return "Monospace";     break;
        case SCRIPT:        return "Script";        break;
        case DECORATIVE:    return "Decorative";    break;
        case SYMBOLIC:      return "Symbolic";      break;
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
