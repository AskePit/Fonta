#include "stylesheet.h"

#include <QObject>

StyleSheet::StyleSheet(const QString& className)
    : className(className)
    , changed(false)
{
}

const QString& StyleSheet::get() const
{
    if(!changed) {
        return sheet;
    }

    sheet = className;
    sheet += "{";

    for (QHash<QString, QString>::const_iterator attr = attributes.begin(); attr != attributes.end(); ++attr) {
        sheet += attr.key();
        sheet += ": ";
        sheet += attr.value();
        sheet += "; ";
    }

    sheet += "}";

    changed = false;
    return sheet;
}

const QString StyleSheet::operator[](const QString& key) const
{
    return attributes.value(key, "");
}


void StyleSheet::set(const QString& key, const QString& val, const QString& unit)
{
    attributes[key] = val + unit;
    changed = true;
}

void StyleSheet::set(const QString& key, int val, const QString& unit)
{
    attributes[key] = QString::number(val) + unit;
    changed = true;
}

void StyleSheet::set(const QString& key, float val, const QString& unit)
{
    attributes[key] = QString::number(val) + unit;
    changed = true;
}

static QString to2Hex(int val) {
    QString hex;

    if(val < 0x10) {
        hex = "0";
    }

    hex += QString::number(val, 16);
    return hex;
}

void StyleSheet::set(const QString& key, int r, int g, int b)
{
    attributes[key] = QString("#") + to2Hex(r) + to2Hex(g) + to2Hex(b);
    changed = true;
}
