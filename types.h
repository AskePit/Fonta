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

struct Version {
    int major;
    int minor;
    int build;
    QString str;

    Version(int major, int minor, int build);
    ~Version() {}
};

inline QFont& mut_font(const QFont& f) { return const_cast<QFont&>(f); }

int getDPI();
inline float pt2px(float pt) { return pt*getDPI()/72.; }
inline float inf() { return std::numeric_limits<float>::infinity(); }

#define decl_constexpr(x) constexpr decltype(x) x

#endif // TYPES_H
