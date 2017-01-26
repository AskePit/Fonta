#ifndef TYPES_H
#define TYPES_H

#include <QString>
#include <QFont>
#include <stdint.h>
#include <limits>


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

using CStringRef = const QString&;
#define cauto const auto&

struct Version {
    int major;
    int minor;
    int build;
    QString str;

    Version(int major, int minor, int build);
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

inline QFont& mut_font(const QFont& f) { return const_cast<QFont&>(f); }

int getDPI();
inline float pt2px(float pt) { return pt*getDPI()/72.; }
inline float inf() { return std::numeric_limits<float>::infinity(); }

int callQuestionDialog(CStringRef message);
void callInfoDialog(CStringRef message);

#define decl_constexpr(x) constexpr decltype(x) x

#define enum_class(x) class x { public: enum type
#define enum_interface };
#define enum_end ;}

#endif // TYPES_H
