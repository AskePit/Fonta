#ifndef LOREMGENERATOR_H
#define LOREMGENERATOR_H

#include "types.h"

namespace fonta {

class LoremGenerator
{
public:
    LoremGenerator(CStringRef text);

    void setText(CStringRef text);
    QString get();
    void unget();
    void reset();
private:
    QString m_text;
    QStringList m_lexemes;
    int m_pos;

    void parse();
};

} // namespace fonta

#endif // LOREMGENERATOR_H
