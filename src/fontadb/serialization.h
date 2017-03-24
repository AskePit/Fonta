#ifndef SERIALIZATION_H
#define SERIALIZATION_H

class QDataStream;

namespace fonta {

class DB;
struct Panose;
struct TTF;
class TTFMap;

QDataStream &operator<<(QDataStream &out, const DB &db);
QDataStream &operator>>(QDataStream &in,        DB &db);

QDataStream &operator<<(QDataStream &out, const TTFMap &m);
QDataStream &operator>>(QDataStream &in,        TTFMap &m);

QDataStream &operator<<(QDataStream &out, const TTF &ttf);
QDataStream &operator>>(QDataStream &in,        TTF &ttf);

QDataStream &operator<<(QDataStream &out, const Panose &p);
QDataStream &operator>>(QDataStream &in,        Panose &p);

} // namespace fonta

#endif // SERIALIZATION_H
