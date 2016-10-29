#ifndef STYLESHEET_H
#define STYLESHEET_H

#include <QString>
#include <QHash>
#include <QColor>

class QObject;

class StyleSheet
{
public:
    StyleSheet(const QString& className);

    const QString& get() const;
    const QString operator[](const QString& key) const;

    void set(const QString& key, const QString& val, const QString& unit = "");
    void set(const QString& key, int val, const QString& unit = "");
    void set(const QString& key, float val, const QString& unit = "");
    void set(const QString& key, int r, int g, int b);

private:
    QString className;
    QHash<QString, QString> attributes;
    mutable QString sheet;
    mutable bool changed;
};

#endif // STYLESHEET_H
