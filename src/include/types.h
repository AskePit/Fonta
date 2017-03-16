#ifndef TYPES_H
#define TYPES_H

#include <QString>

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

#define enum_class(x) class x { public: enum type
#define enum_interface };
#define enum_end ;}

#endif // TYPES_H
