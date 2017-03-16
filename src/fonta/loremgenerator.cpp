#include "loremgenerator.h"

#include <QString>

namespace fonta {

LoremGenerator::LoremGenerator(CStringRef text, QChar delim)
    : m_text(text)
    , m_delim(delim)
    , m_pos(0)
{
    parse();
}

void LoremGenerator::parse()
{
    m_lexemes = m_text.split(m_delim);
}

void LoremGenerator::setText(CStringRef text)
{
    m_text = text;
    parse();
    reset();
}

QString LoremGenerator::get()
{
    QString res = m_lexemes[m_pos++] + m_delim;

    if(m_pos >= m_lexemes.length()) {
        m_pos = 0;
    }

    return res;
}

void LoremGenerator::unget()
{
    --m_pos;
    if(m_pos < 0) {
        m_pos = m_lexemes.length()-1;
    }
}

void LoremGenerator::reset()
{
    m_pos = 0;
}

} // namespace fonta
