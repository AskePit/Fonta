#include "fontawidgets.h"

#include "sampler.h"

#include <QPainter>
#include <QTextBlock>
#include <QListWidget>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <fontadb.h>
#include <iostream>

#include <qdebug.h>

int callQuestionDialog(CStringRef message)
{
    QMessageBox msgBox;
    msgBox.setText(message);

    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    return msgBox.exec();
}

void callInfoDialog(CStringRef message)
{
    QMessageBox msgBox;
    msgBox.setText(message);

    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}

namespace fonta {

About::About(const Version& version, QWidget *parent) :
    QDialog(parent)
{
    resize(175, 85);

    auto vLayout = new QVBoxLayout(this);
    auto vSpacer3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vLayout->addItem(vSpacer3);

    auto label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter);
    vLayout->addWidget(label);

    auto vSpacer2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vLayout->addItem(vSpacer2);

    auto hLayout = new QHBoxLayout();
    auto horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hLayout->addItem(horizontalSpacer);

    auto pushButton = new QPushButton(this);
    pushButton->setObjectName(QStringLiteral("pushButton"));
    hLayout->addWidget(pushButton);

    auto hSpacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hLayout->addItem(hSpacer2);
    vLayout->addLayout(hLayout);

    auto verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vLayout->addItem(verticalSpacer);

    setWindowTitle(tr("About"));
    label->setText(tr("Fonta v. %1").arg(version.str));
    pushButton->setText(tr("OK"));

    QMetaObject::connectSlotsByName(this);
}

About::~About()
{
}

void About::on_pushButton_clicked()
{
    hide();
}

RenameTabEdit::RenameTabEdit(QTabWidget* tabWidget, WorkArea* workArea, QWidget *parent)
    : QLineEdit(parent)
    , m_tabWidget(tabWidget)
    , m_workArea(workArea)
{
    setWindowFlags(Qt::FramelessWindowHint);

    const QRect& r = tabWidget->tabBar()->tabRect(workArea->id());
    setGeometry(r);
    setText(workArea->name());
    selectAll();
    setFocus();
}

void RenameTabEdit::keyPressEvent(QKeyEvent* event)
{
    int key = event->key();
    if(key == Qt::Key_Return) {
        apply();
    } else {
        QLineEdit::keyPressEvent(event);
    }
}

void RenameTabEdit::apply()
{
    QString txt = text();
    m_workArea->rename(txt);
    m_tabWidget->setTabText(m_workArea->id(), txt);
    emit applied();
    deleteLater();
}

TooglePanel::TooglePanel(QWidget* parent)
    : QWidget(parent)
{}


void TooglePanel::toogle(bool toogle)
{
    if(toogle) {
        setStyleSheet("background-color:gray;");
    } else {
        setStyleSheet("background-color:white;");
    }
}

void TooglePanel::paintEvent(QPaintEvent *pe)
{
    (void)pe;

    QStyleOption o;
    o.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(
    QStyle::PE_Widget, &o, &p, this);
}

constexpr bool Field::showBorders;

Field::Field(InitType initType, QWidget* parent)
    : QTextEdit(parent)
    , m_fontStyle("Normal")
    , m_preferableFontStyle("Normal")
    , m_leading(inf())
    , m_tracking(0)
    , m_sheet("QTextEdit")
    , m_userChangedText(false)
{
    setFrameShape(QFrame::Box);
    setFrameShadow(QFrame::Plain);
    setLineWidth(showBorders ? 1 : 0);
    setAcceptRichText(false);
    setLeading(m_leading);
    alignText(Qt::AlignLeft);

    if(initType == InitType::Sampled) {
        setSamples(Sampler::getText(), Sampler::getRusText());
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

void Field::toogle(bool toogle) { return m_tooglePanel->toogle(toogle); }
QWidget* Field::surfaceWidget() { return m_surfaceWidget; }

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

void Field::setSamples(CStringRef latin, CStringRef rus)
{
    m_latinText = latin;
    m_rusText = rus;
}

void Field::focusInEvent(QFocusEvent* e)
{
    QTextEdit::focusInEvent(e);
    emit(focussed(this));
}

void Field::keyPressEvent(QKeyEvent *k)
{
    Q_UNUSED(k);

    bool ctrlChange = k->modifiers() == Qt::ControlModifier
                  && (k->key() == Qt::Key_X || k->key() == Qt::Key_V);

    // all printable keys
    if((k->modifiers() != Qt::ControlModifier || ctrlChange)
    && k->modifiers() != Qt::AltModifier
    && k->key() >= Qt::Key_Space
    && k->key() <= Qt::Key_ydiaeresis) {
        m_userChangedText = true;
    }
    QTextEdit::keyPressEvent(k);
}

void Field::setFontFamily(CStringRef family)
{
    /*if(font().family() == family) {
        return;
    }*/

    if(!m_userChangedText) {
        bool cyr = fontaDB().isCyrillic(family);

        if(cyr) {
            setText(m_rusText);
        } else {
            setText(m_latinText);
        }
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
}

void Field::setFontSize(float size)
{
    QFont newFont(font());
    newFont.setPointSizeF(size);

    float px = pt2px(newFont.pointSize())/1000.0f*(float)m_tracking;
    newFont.setLetterSpacing(QFont::AbsoluteSpacing, px);

    setFont(newFont);
}

void Field::setFontStyle(CStringRef style)
{
    const QFont& f = font();

    QFont newFont = qtDB().font(f.family(), style, f.pointSize());

    float px = pt2px(newFont.pointSize())/1000.0f*(float)m_tracking;
    newFont.setLetterSpacing(QFont::AbsoluteSpacing, px);

    setFont(newFont);
    m_fontStyle = style;
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
}

WorkArea::WorkArea(int id, QWidget* parent, QString name)
    : QSplitter(parent)
    , m_id(id)
    , m_name(name)
{
    QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Expanding);
    sp.setHorizontalStretch(2);
    sp.setVerticalStretch(0);
    sp.setHeightForWidth(sizePolicy().hasHeightForWidth());
    setSizePolicy(sp);
    setAutoFillBackground(true);
    setLineWidth(0);
    setOrientation(Qt::Vertical);
    setHandleWidth(0);
    setChildrenCollapsible(false);
}

void WorkArea::createSample()
{
    Sampler::loadSample(*this);

    m_fields[0]->setFocus();
    m_currField = m_fields[0];
}

WorkArea::~WorkArea()
{
    clear();
}

int WorkArea::id() const
{
    return m_id;
}

void WorkArea::setId(int id)
{
    m_id = id;
}

CStringRef WorkArea::name() const
{
    return m_name;
}

void WorkArea::rename(CStringRef name)
{
    m_name = name;
}

Field* WorkArea::currField() const
{
    return m_currField;
}

void WorkArea::setCurrField(Field* field)
{
    m_currField = field;
}

Field* WorkArea::addField(InitType initType)
{    
    Field* field = new Field(initType, this);

    int id = m_fields.length();

    field->setId(id);
    m_fields.push_back(field);

    addWidget(field->surfaceWidget());

    connect(field, &Field::focussed, this, &WorkArea::on_currentFieldChanged);

    return field;
}

void WorkArea::popField()
{
    delete m_fields.last()->surfaceWidget();
    m_fields.pop_back();
}

Field* WorkArea::operator[](int i)
{
    return m_fields.at(i);
}

int WorkArea::fieldCount() const
{
    return m_fields.size();
}

void WorkArea::clear()
{
    int size = m_fields.length();
    for(int i = 0; i<size; ++i) {
        popField();
    }
}

void WorkArea::on_currentFieldChanged(Field* changedField)
{
    for(auto field : m_fields) {
        if(field == changedField) {
            m_currField = field;
            field->toogle(true);
        } else {
            field->toogle(false);
        }
    }
}

void WorkArea::save(QJsonObject &json) const
{
    json["id"] = id();
    json["name"] = name();

    QJsonArray fields;
    for(auto field : m_fields) {
        QJsonObject fieldObj;
        field->save(fieldObj);
        fields.append(fieldObj);
    }
    json["fields"] = fields;

    json["currField"] = m_currField->id();

    QJsonArray sizesArr;
    for(int s : sizes()) {
        sizesArr.append(s);
    }
    json["sizes"] = sizesArr;
}

void WorkArea::loadSample(CStringRef jsonTxt)
{
    clear();

    QByteArray data = jsonTxt.toUtf8();
    QJsonObject json = QJsonDocument::fromJson(data).object();

    QJsonArray fields = json["fields"].toArray();
    for(const QJsonValue& fieldVal : fields) {
        Field* field = addField();
        field->load(fieldVal.toObject());
    }

    if(m_fields.length()) {
        m_fields.at(0)->setFocus();
    }

    QJsonArray sizes = json["sizes"].toArray();
    QList<int> sizesList;
    for(const QJsonValue& size : sizes) {
        sizesList << size.toInt(50);
    }
    setSizes(sizesList);
}

void WorkArea::load(const QJsonObject &json)
{
    clear();

    m_id = json["id"].toInt(0);
    m_name = json["name"].toString("");

    QJsonArray fields = json["fields"].toArray();
    for(const QJsonValue& fieldVal : fields) {
        Field* field = addField();
        field->load(fieldVal.toObject());
    }

    if(m_fields.length()) {
        int fieldId = json["currField"].toInt(0);
        m_currField = m_fields.at(fieldId);
    }

    QJsonArray sizes = json["sizes"].toArray();
    QList<int> sizesList;
    for(const QJsonValue& size : sizes) {
        sizesList << size.toInt(50);
    }
    setSizes(sizesList);
}

FilterEdit::FilterEdit(QWidget* parent)
    : QLineEdit(parent)
{}

void FilterEdit::keyPressEvent(QKeyEvent* event)
{
    int key = event->key();

    if(key >= Qt::Key_Space &&  key <= Qt::Key_questiondown) {
        suppose(QChar((char)key));
    } else if(key == Qt::Key_Return) {
        apply();
    } else {
        QLineEdit::keyPressEvent(event);
    }
}

void FilterEdit::mousePressEvent(QMouseEvent * e)
{
    (void)e;
    selectAll();
}

void FilterEdit::suppose(QChar typed)
{
    int selectStart = selectionStart();

    QString match;
    if(selectStart == -1) {
        selectStart = cursorPosition();
        match = text();
        match.insert(selectStart, typed);
    } else {
        match = text().mid(0, selectStart) + typed;
    }

    for(int i = 0; i < m_listWidget->count(); ++i)
    {
        QListWidgetItem* item = m_listWidget->item(i);
        CStringRef fontName = item->text();

        if(fontName.startsWith(match, Qt::CaseInsensitive)) {
            setText(fontName);

            ++selectStart;
            setSelection(selectStart, fontName.size()-selectStart);
            break;
        }
    }
}

void FilterEdit::apply()
{
    QList<QListWidgetItem*> items = m_listWidget->findItems(text(), Qt::MatchExactly);
    if(items.size() > 0) {
        m_listWidget->setCurrentItem(items[0]);
        m_listWidget->scrollToItem(items[0], QAbstractItemView::PositionAtCenter);
    }
}

ComboBox::ComboBox(QWidget* parent)
    : QComboBox(parent)
{
    setLineEdit(new ComboBoxLineEdit());
}

void ComboBox::wheelEvent(QWheelEvent* e)
{
    int delta = e->angleDelta().y();

    int currVal = strtol(currentText().toStdString().c_str(), nullptr, 10);

    for(int i = 0; i<count(); ++i) {
        if(i == count()-1) {
            continue;
        }

        CStringRef iStr = itemText(i);
        CStringRef i1Str = itemText(i+1);
        int iVal = strtol(iStr.toStdString().c_str(), nullptr, 10);
        int i1Val = strtol(i1Str.toStdString().c_str(), nullptr, 10);

        // scroll up - up the value
        if(delta <= 0) {
            if(iVal < currVal && i1Val >= currVal) {
                setCurrentText(iStr);
                lineEdit()->returnPressed();
            }
        }
        // scroll down - down the value
        else {
            if(iVal <= currVal && i1Val > currVal) {
                setCurrentText(i1Str);
                lineEdit()->returnPressed();
            }
        }
    }
}

ComboBoxLineEdit::ComboBoxLineEdit(QWidget* parent)
    : QLineEdit(parent)
{}

void ComboBoxLineEdit::mousePressEvent(QMouseEvent* e)
{
    (void)e;
    selectAll();
}

} // namespace fonta
