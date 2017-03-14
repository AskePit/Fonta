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
