#include "field.h"

#include "togglepanel.h"
#include "sampler.h"
#include "fontadb.h"
#include "loremgenerator.h"

#include <QHBoxLayout>
#include <QJsonObject>
#include <QScrollBar>
#include <QTimerEvent>

namespace fonta {

Field::Field(InitType initType, QWidget* parent)
    : QTextEdit(parent)
    , m_fontStyle("Normal")
    , m_preferableFontStyle("Normal")
    , m_leading(inf())
    , m_tracking(0)
    , m_sheet("QTextEdit")
    , m_contentMode(ContentMode::News)
    , m_languageContext(LanguageContext::Auto)
{
    setFrameShape(QFrame::Box);
    setFrameShadow(QFrame::Plain);
    setLineWidth(0);
    setAcceptRichText(false);
    setLeading(m_leading);
    alignText(Qt::AlignLeft);

    if(initType == InitType::Sampled) {
        fetchSamples();
        setFontSize(10);
        setFontFamily("Arial");
    }

    m_sheet.set("padding-top", "0px");
    m_sheet.set("padding-left", "15px");
    m_sheet.set("padding-right", "15px");
    m_sheet.set("padding-bottom", "0px");
    m_sheet.set("min-height", "25px");
    m_sheet.set("background-color", "white");
    applySheet();

    m_surfaceWidget = new QWidget();
    m_surfaceLayout = new QHBoxLayout(m_surfaceWidget);
    m_surfaceLayout->setSpacing(0);
    m_surfaceLayout->setContentsMargins(0, 0, 0, 0);

    m_tooglePanel = new TooglePanel(m_surfaceWidget);
    m_tooglePanel->setMinimumSize(QSize(5, 0));

    m_surfaceLayout->addWidget(m_tooglePanel);
    m_surfaceLayout->addWidget(this);
}

void Field::toogle(bool toogle)
{
    return m_tooglePanel->toogle(toogle);
}

QWidget* Field::surfaceWidget()
{
    return m_surfaceWidget;
}

int Field::id() const
{
    return m_id;
}

QString Field::fontFamily() const
{
    return font().family();
}

float Field::fontSize() const
{
    return font().pointSizeF();
}

QString Field::fontStyle() const
{
    return m_fontStyle;
}

QString Field::preferableFontStyle() const
{
    return m_preferableFontStyle;
}

Qt::Alignment Field::textAlignment() const
{
    return m_alignment;
}

float Field::leading() const
{
    return m_leading;
}

int Field::tracking() const
{
    return m_tracking;
}

ContentMode Field::contentMode()
{
    return m_contentMode;
}

LanguageContext Field::languageContext()
{
    return m_languageContext;
}

void Field::setId(int id)
{
    m_id = id;
}

StyleSheet& Field::sheet() const
{
    return m_sheet;
}

void Field::applySheet()
{
    setStyleSheet(m_sheet.get());
}

void Field::fetchSamples()
{
    m_engText = Sampler::getEngText(m_contentMode);
    m_rusText = Sampler::getRusText(m_contentMode);
}

static QString truncWord(CStringRef str)
{
    int pos = str.lastIndexOf(' ', -2);

    if(pos != -1) {
        return str.left(pos);
    } else {
        return "";
    }
}

void Field::updateText()
{
    if(m_contentMode == ContentMode::UserDefined) {
        return;
    }

    QString text;

    switch(m_languageContext) {
        default:
        case LanguageContext::Auto: {
            bool cyr = fontaDB().isCyrillic(fontFamily());
            text = cyr ? m_rusText : m_engText;
        } break;
        case LanguageContext::Eng: text = m_engText; break;
        case LanguageContext::Rus: text = m_rusText; break;
    }

    if(m_contentMode == ContentMode::LoremIpsum) {
        clear();
        LoremGenerator g(text, '\n');

        if(!verticalScrollBar()->isVisible()) {
            while(!verticalScrollBar()->isVisible()) {
                setText(toPlainText() + g.get());
            }
        }

        while(verticalScrollBar()->isVisible()) {
            QString s = truncWord(toPlainText());
            setText(s);

            if(s.isEmpty()) {
                break;
            }
        }
    } else {
        setText(text);
    }

    // text alignment is reseted after setText();
    alignText(textAlignment());
}

void Field::focusInEvent(QFocusEvent* e)
{
    QTextEdit::focusInEvent(e);
    emit(focussed(this));
}

void Field::keyPressEvent(QKeyEvent *k)
{
    QString oldText = toPlainText();
    QTextEdit::keyPressEvent(k);
    QString newText = toPlainText();

    if(oldText != newText) {
        if(m_contentMode != ContentMode::UserDefined) {
            m_contentMode = ContentMode::UserDefined;
            emit contentBecameUserDefined();
        }
    }
}

void Field::resizeEvent(QResizeEvent* e)
{
    QTextEdit::resizeEvent(e);

    if (m_timerId) {
        killTimer(m_timerId);
        m_timerId = 0;
    }

    if(m_contentMode == ContentMode::LoremIpsum) {
        m_timerId = startTimer(250);
    }
}

void Field::timerEvent(QTimerEvent* e)
{
    if(m_contentMode == ContentMode::LoremIpsum) {
        updateText();
    }

    killTimer(e->timerId());
    m_timerId = 0;
}

void Field::setFontFamily(CStringRef family)
{
    if(font().family() == family) {
        return;
    }

    QFont newFont(font());
    newFont.setFamily(family);
    setFont(newFont);

    const QStringList& s = fontaDB().styles(family);

    CStringRef prefStyle = preferableFontStyle();
    bool found = s.contains(prefStyle);
    if(found) {
        setFontStyle(prefStyle);
    } else if (s.length()) {
        setFontStyle(s.at(0));
    }

    updateText();
}

void Field::setFontSize(float size)
{
    QFont newFont(font());
    newFont.setPointSizeF(size);

    float px = pt2px(newFont.pointSize())/1000.0f*(float)m_tracking;
    newFont.setLetterSpacing(QFont::AbsoluteSpacing, px);

    setFont(newFont);
    updateText();
}

void Field::setFontStyle(CStringRef style)
{
    const QFont& f = font();

    QFont newFont = qtDB().font(f.family(), style, f.pointSize());

    float px = pt2px(newFont.pointSize())/1000.0f*(float)m_tracking;
    newFont.setLetterSpacing(QFont::AbsoluteSpacing, px);

    setFont(newFont);
    m_fontStyle = style;
    updateText();
}

void Field::setPreferableFontStyle(CStringRef style)
{
    m_preferableFontStyle = style;
    setFontStyle(style);
}

void Field::alignText(Qt::Alignment alignment)
{
    m_alignment = alignment;
    alignTextHorizontally(alignment);
}

void Field::alignTextHorizontally(Qt::Alignment alignment)
{
    QTextCursor cursor(textCursor());
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    QTextBlockFormat format;
    format.setAlignment(alignment);
    cursor.mergeBlockFormat(format);
}

void Field::setLeading(float val)
{
    m_leading = val;

    QTextCursor cursor(textCursor());
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    QTextBlockFormat format;

    if(val == inf()) {
        format.setLineHeight(120, QTextBlockFormat::ProportionalHeight);
    } else {
        format.setLineHeight(val/font().pointSizeF()*100, QTextBlockFormat::ProportionalHeight);
    }

    cursor.mergeBlockFormat(format);
}

void Field::setTracking(int val)
{
    m_tracking = val;

    QFont newFont(font());

    float px = pt2px(newFont.pointSizeF())*0.001f*(float)val;
    newFont.setLetterSpacing(QFont::AbsoluteSpacing, px);
    newFont.setKerning(true);

    setFont(newFont);
    updateText();
}

void Field::setContentMode(ContentMode mode)
{
    m_contentMode = mode;
    fetchSamples();
    updateText();
}

void Field::setLanguageContext(LanguageContext context)
{
    m_languageContext = context;
    updateText();
}

void Field::save(QJsonObject &json) const
{
    const QFont& f = font();
    json["family"] = f.family();
    json["style"] = fontStyle();
    json["size"] = f.pointSizeF();
    json["leading"] = leading();
    json["tracking"] = tracking();
    json["alignment"] = static_cast<int>(textAlignment());
    json["text"] = toPlainText();
    json["textColor"] = sheet()["color"];
    json["backgroundColor"] = sheet()["background-color"];
}

void Field::load(const QJsonObject &json)
{
    QString family = json["family"].toString("Arial");
    double size = json["size"].toDouble(12.0);
    QString style = json["style"].toString("Normal");

    setPreferableFontStyle(style);

    QFont newFont = qtDB().font(family, style, size); // requires int size
    newFont.setPointSizeF(size);                      // set double size
    setFont(newFont);

    setText(json["text"].toString("The quick brown fox jumped over the lazy dog"));
    alignText(static_cast<Qt::Alignment>(json["alignment"].toInt(1)));
    setLeading(json["leading"].toDouble(inf()));
    setTracking(json["tracking"].toInt(0));

    sheet().set("color", json["textColor"].toString());
    sheet().set("background-color", json["backgroundColor"].toString());
    applySheet();

    m_contentMode = ContentMode::UserDefined;
    m_languageContext = LanguageContext::Auto;
}

} // namespace fonta
