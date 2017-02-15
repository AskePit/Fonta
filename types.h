#ifndef TYPES_H
#define TYPES_H

#include <QString>
#include <QFont>
#include <limits>

typedef quint8 u8;
typedef quint16 u16;
typedef quint32 u32;
typedef quint64 u64;

typedef qint8 i8;
typedef qint16 i16;
typedef qint32 i32;
typedef qint64 i64;

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
