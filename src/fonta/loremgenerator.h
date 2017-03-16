#ifndef LOREMGENERATOR_H
#define LOREMGENERATOR_H

#include "types.h"
#include <QStringList>

namespace fonta {

class LoremGenerator
{
public:
    LoremGenerator(CStringRef text, QChar delim);

    void setText(CStringRef text);
    QString get();
    void unget();
    void reset();
private:
    QString m_text;
    QChar m_delim;
    QStringList m_lexemes;
    int m_pos;

    void parse();
};

} // namespace fonta

#endif // LOREMGENERATOR_H
