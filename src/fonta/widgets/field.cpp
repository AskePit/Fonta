#include "field.h"

#include "utils.h"
#include "togglepanel.h"
#include "sampler.h"
#include "fontadb.h"
#include "loremgenerator.h"

#include <QHBoxLayout>
#include <QJsonObject>
#include <QScrollBar>
#include <QTimerEvent>
#include <QMenu>
#include <QDebug>

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
    , m_timerId(0)
{
    setFrameShape(QFrame::Box);
    setFrameShadow(QFrame::Plain);
    setLineWidth(0);
    setAcceptRichText(false);

    alignText(Qt::AlignLeft);

    if(initType == InitType::Sampled) {
        fetchSamples();
        setFontSize(10);
        setFontFamily(QStringLiteral("Arial"));
    }

    m_sheet.set(QStringLiteral("padding-top"), QStringLiteral("0px"));
    m_sheet.set(QStringLiteral("padding-left"), QStringLiteral("15px"));
    m_sheet.set(QStringLiteral("padding-right"), QStringLiteral("15px"));
    m_sheet.set(QStringLiteral("padding-bottom"), QStringLiteral("0px"));
    m_sheet.set(QStringLiteral("min-height"), QStringLiteral("25px"));
    m_sheet.set(QStringLiteral("background-color"), QStringLiteral("white"));
    applySheet();

    m_surfaceWidget = new QWidget();
    m_surfaceLayout = new QHBoxLayout(m_surfaceWidget);
    m_surfaceLayout->setSpacing(0);
    m_surfaceLayout->setContentsMargins(0, 0, 0, 0);

    m_tooglePanel = new TooglePanel(m_surfaceWidget);
    m_tooglePanel->setMinimumSize(QSize(5, 0));

    m_surfaceLayout->addWidget(m_tooglePanel);
    m_surfaceLayout->addWidget(this);

    setLeading(m_leading);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTextEdit::customContextMenuRequested, this, &Field::showContextMenu);
}

Field& Field::operator=(const Field &other)
{
    m_fontStyle = other.m_fontStyle;
    m_preferableFontStyle = other.m_preferableFontStyle;
    m_leading = other.m_leading;
    m_tracking = other.m_tracking;
    m_sheet = other.m_sheet;
    m_contentMode = other.m_contentMode;
    m_languageContext = other.m_languageContext;
    m_engText = other.m_engText;
    m_rusText = other.m_rusText;
    m_alignment = other.alignment();

    setText(other.toPlainText());
    setFont(other.font());
    alignText(m_alignment);
    updateLeading();

    return *this;
}

Field* Field::clone() const
{
    Field* f = new Field(InitType::Empty);

    *f = *this;
    return f;
}

void Field::toogle(bool toogle)
{
    return m_tooglePanel->toogle(toogle);
}

QWidget* Field::surfaceWidget()
{
    return m_surfaceWidget;
}

void Field::swapFamiliesWith(Field *other)
{
    Field *f1 = this;
    Field *f2 = other;

    QString family1 = f1->fontFamily();
    QString family2 = f2->fontFamily();

    f1->setFontFamily(family2);
    f2->setFontFamily(family1);
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
    m_engText = Sampler::instance()->getEngText(m_contentMode);
    m_rusText = Sampler::instance()->getRusText(m_contentMode);
}

static QString truncWord(CStringRef str)
{
    int pos = str.lastIndexOf(' ', -2);

    if(pos != -1) {
        return str.left(pos);
    } else {
        return QStringLiteral("");
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
        updateLoremText();
    } else {
        setText(text);
        updateLeading();
        alignText(textAlignment()); // text alignment is reseted after setText()
    }
}

void Field::updateLoremText()
{
    if(m_contentMode != ContentMode::LoremIpsum) {
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

    clear();
    LoremGenerator g(text, '\n');

    if(!verticalScrollBar()->isVisible()) {
        while(!verticalScrollBar()->isVisible()) {
            setText(toPlainText() + g.get());
            // leading is reseted after setText();
            updateLeading();
        }
    }

    while(verticalScrollBar()->isVisible()) {
        QString s = truncWord(toPlainText());
        setText(s);
        updateLeading();

        if(s.isEmpty()) {
            break;
        }
    }

    // text alignment is reseted after setText();
    alignText(textAlignment());
}

void Field::focusInEvent(QFocusEvent* e)
{
    QTextEdit::focusInEvent(e);
    emit(focussed());
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
        // leading change cause undesired resizeEvent which cause superfluous updateText() call (in Field::timerEvent) which erases leading
        updateLoremText();
    }

    if (m_timerId) {
        killTimer(e->timerId());
        m_timerId = 0;
    }
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
    updateLoremText();
}

void Field::setFontStyle(CStringRef style)
{
    const QFont& f = font();

    QFont newFont = qtDB().font(f.family(), style, f.pointSize());

    float px = pt2px(newFont.pointSize())/1000.0f*(float)m_tracking;
    newFont.setLetterSpacing(QFont::AbsoluteSpacing, px);

    setFont(newFont);
    m_fontStyle = style;
    updateLoremText();
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
    updateLoremText();
    updateLeading();
}

void Field::updateLeading()
{
    QTextCursor cursor(textCursor());
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    QTextBlockFormat format;

    qreal lineHeight = (m_leading == inf()) ? 120 : m_leading/font().pointSizeF()*100;
    format.setLineHeight(lineHeight, QTextBlockFormat::ProportionalHeight);

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
    updateLoremText();
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
    json[QLatin1String("family")] = f.family();
    json[QLatin1String("style")] = fontStyle();
    json[QLatin1String("size")] = f.pointSizeF();
    json[QLatin1String("leading")] = leading();
    json[QLatin1String("tracking")] = tracking();
    json[QLatin1String("alignment")] = static_cast<int>(textAlignment());
    json[QLatin1String("text")] = toPlainText();
    json[QLatin1String("textColor")] = sheet()[QStringLiteral("color")];
    json[QLatin1String("backgroundColor")] = sheet()[QStringLiteral("background-color")];
}

void Field::load(const QJsonObject &json)
{
    QString family = json[QLatin1String("family")].toString(QStringLiteral("Arial"));
    double size = json[QLatin1String("size")].toDouble(12.0);
    QString style = json[QLatin1String("style")].toString(QStringLiteral("Normal"));

    setPreferableFontStyle(style);

    QFont newFont = qtDB().font(family, style, size); // requires int size
    newFont.setPointSizeF(size);                      // set double size
    setFont(newFont);

    setText(json[QLatin1String("text")].toString(QStringLiteral("The quick brown fox jumped over the lazy dog")));
    alignText(static_cast<Qt::Alignment>(json[QLatin1String("alignment")].toInt(1)));
    setLeading(json[QLatin1String("leading")].toDouble(inf()));
    setTracking(json[QLatin1String("tracking")].toInt(0));

    sheet().set(QStringLiteral("color"), json[QLatin1String("textColor")].toString());
    sheet().set(QStringLiteral("background-color"), json[QLatin1String("backgroundColor")].toString());
    applySheet();

    m_contentMode = ContentMode::UserDefined;
    m_languageContext = LanguageContext::Auto;
}

void Field::showContextMenu(const QPoint &point)
{
    QMenu menu;

    QAction swapAction(tr("Swap Fonts"), this);
    connect(&swapAction, &QAction::triggered, this, &Field::swapRequested);

    menu.addAction(&swapAction);
    menu.exec(mapToGlobal(point));
}

} // namespace fonta
