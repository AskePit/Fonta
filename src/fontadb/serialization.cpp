#include "serialization.h"

#include "fontadb.h"
#include <QDataStream>


namespace fonta {

QDataStream &operator<<(QDataStream &out, const DB &db)
{
    out << db.TTFs;

    return out;
}

QDataStream &operator>>(QDataStream &in, DB &db)
{
    in >> db.TTFs;

    return in;
}

QDataStream &operator<<(QDataStream &out, const TTFMap &m)
{
    out << (u64)m.size();
    for(cauto pair : m) {
        out << pair.first;
        out << pair.second;
    }

    return out;
}

QDataStream &operator>>(QDataStream &in, TTFMap &m)
{
    u64 size = 0;
    in >> size;
    for(u64 i = 0; i<size; ++i) {
        QString name;
        TTF ttf;
        in >> name;
        in >> ttf;

        m[name] = std::move(ttf);
    }

    return in;
}

QDataStream &operator<<(QDataStream &out, const TTF &ttf)
{
    out << static_cast<i32>(ttf.familyClass);
    out << static_cast<i32>(ttf.familySubClass);
    out << ttf.latin;
    out << ttf.cyrillic;
    out << ttf.panose;
    out << ttf.valid;
    out << ttf.files;
    out << ttf.linkedFonts;

    return out;
}

QDataStream &operator>>(QDataStream &in, TTF &ttf)
{
    i32 familyClass = 0;
    i32 familySubClass = 0;

    in >> familyClass;
    in >> familySubClass;
    in >> ttf.latin;
    in >> ttf.cyrillic;
    in >> ttf.panose;
    in >> ttf.valid;
    in >> ttf.files;
    in >> ttf.linkedFonts;

    ttf.familyClass = static_cast<FamilyClass::type>(familyClass);
    ttf.familySubClass = static_cast<int>(familyClass);

    return in;
}

QDataStream &operator<<(QDataStream &out, const Panose &p)
{
    out << p.Family;
    out << p.SerifStyle;
    out << p.Weight;
    out << p.Proportion;
    out << p.Contrast;
    out << p.Stroke;
    out << p.ArmStyle;
    out << p.LetterForm;
    out << p.MidLine;
    out << p.XHeight;

    return out;
}

QDataStream &operator>>(QDataStream &in, Panose &p)
{
    in >> p.Family;
    in >> p.SerifStyle;
    in >> p.Weight;
    in >> p.Proportion;
    in >> p.Contrast;
    in >> p.Stroke;
    in >> p.ArmStyle;
    in >> p.LetterForm;
    in >> p.MidLine;
    in >> p.XHeight;

    return in;
}

} // namespace fonta
