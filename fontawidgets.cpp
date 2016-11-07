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

About::About(const Version& version, QWidget *parent) :
    QDialog(parent)
{
    resize(175, 85);
    verticalLayout = new QVBoxLayout(this);
    verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayout->addItem(verticalSpacer_3);

    label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter);

    verticalLayout->addWidget(label);

    verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayout->addItem(verticalSpacer_2);

    horizontalLayout = new QHBoxLayout();
    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout->addItem(horizontalSpacer);

    pushButton = new QPushButton(this);
    pushButton->setObjectName(QStringLiteral("pushButton"));

    horizontalLayout->addWidget(pushButton);

    horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout->addItem(horizontalSpacer_2);


    verticalLayout->addLayout(horizontalLayout);

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayout->addItem(verticalSpacer);


    setWindowTitle("About");
    label->setText("Fonta v. " + version.str);
    pushButton->setText("OK");

    QMetaObject::connectSlotsByName(this);
}

About::~About()
{
}

void About::on_pushButton_clicked()
{
    hide();
}

RenameTabEdit::RenameTabEdit(QTabWidget* tabWidget, FontaWorkArea* workArea, QWidget *parent)
    : QLineEdit(parent)
    , tabWidget(tabWidget)
    , workArea(workArea)
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
    workArea->rename(txt);
    tabWidget->setTabText(workArea->id(), txt);
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

constexpr bool FontaField::showBorders;

FontaField::FontaField(QWidget* parent)
    : QTextEdit(parent)
    , m_fontStyle("Normal")
    , m_preferableFontStyle("Normal")
    , m_leading(inf())
    , m_tracking(0)
    , m_sheet("QTextEdit")
{
    setFrameShape(QFrame::Box);
    setFrameShadow(QFrame::Plain);
    setLineWidth(showBorders ? 1 : 0);
    setAcceptRichText(false);
    setFont(QFont("Arial", 10));
    setText(Sampler::getText());
    setLeading(m_leading);
    alignText(Qt::AlignLeft);

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


void FontaField::focusInEvent(QFocusEvent* e)
{
    QTextEdit::focusInEvent(e);
    emit(focussed(this));
}

void FontaField::setFontFamily(CStringRef family)
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
}

void FontaField::setFontSize(float size)
{
    QFont newFont(font());
    newFont.setPointSizeF(size);

    float px = pt2px(newFont.pointSize())/1000.0f*(float)m_tracking;
    newFont.setLetterSpacing(QFont::AbsoluteSpacing, px);

    setFont(newFont);
}

void FontaField::setFontStyle(CStringRef style)
{
    const QFont& f = font();

    QFont newFont = qtDB().font(f.family(), style, f.pointSize());

    float px = pt2px(newFont.pointSize())/1000.0f*(float)m_tracking;
    newFont.setLetterSpacing(QFont::AbsoluteSpacing, px);

    setFont(newFont);
    m_fontStyle = style;
}

void FontaField::setPreferableFontStyle(CStringRef style)
{
    m_preferableFontStyle = style;
    setFontStyle(style);
}

void FontaField::alignText(Qt::Alignment alignment)
{
    m_alignment = alignment;
    alignTextHorizontally(alignment);
}

void FontaField::alignTextHorizontally(Qt::Alignment alignment)
{
    QTextCursor cursor(textCursor());
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    QTextBlockFormat format;
    format.setAlignment(alignment);
    cursor.mergeBlockFormat(format);
}

void FontaField::setLeading(float val)
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

void FontaField::setTracking(int val)
{
    m_tracking = val;

    QFont newFont(font());

    float px = pt2px(newFont.pointSizeF())*0.001f*(float)val;
    newFont.setLetterSpacing(QFont::AbsoluteSpacing, px);
    newFont.setKerning(true);

    setFont(newFont);
}

void FontaField::save(QJsonObject &json) const
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

void FontaField::load(const QJsonObject &json)
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

FontaWorkArea::FontaWorkArea(int id, QWidget* parent, QString name)
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

void FontaWorkArea::createSample()
{
    Sampler::loadSample(*this);

    m_fields[0]->setFocus();
    m_currField = m_fields[0];
}

FontaWorkArea::~FontaWorkArea()
{
    clear();
}

FontaField* FontaWorkArea::addField()
{    
    FontaField* field = new FontaField(this);

    int id = m_fields.length();

    field->setId(id);
    m_fields.push_back(field);

    addWidget(field->surfaceWidget());

    connect(field, SIGNAL(focussed(FontaField*)), this, SLOT(on_currentFieldChanged(FontaField*)));

    return field;
}

void FontaWorkArea::popField()
{
    delete m_fields.last()->surfaceWidget();
    m_fields.pop_back();
}

FontaField* FontaWorkArea::operator[](int i)
{
    return m_fields.at(i);
}

void FontaWorkArea::clear()
{
    int size = m_fields.length();
    for(int i = 0; i<size; ++i) {
        popField();
    }
}

void FontaWorkArea::on_currentFieldChanged(FontaField* changedField)
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

void FontaWorkArea::save(QJsonObject &json) const
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

void FontaWorkArea::loadSample(CStringRef jsonTxt)
{
    clear();

    QByteArray data = jsonTxt.toUtf8();
    QJsonObject json = QJsonDocument::fromJson(data).object();

    QJsonArray fields = json["fields"].toArray();
    for(const QJsonValue& fieldVal : fields) {
        FontaField* field = addField();
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

void FontaWorkArea::load(const QJsonObject &json)
{
    clear();

    m_id = json["id"].toInt(0);
    m_name = json["name"].toString("");

    QJsonArray fields = json["fields"].toArray();
    for(const QJsonValue& fieldVal : fields) {
        FontaField* field = addField();
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

FontaFilterEdit::FontaFilterEdit(QListWidget* listWidget, QWidget* parent)
    : QLineEdit(parent)
    , listWidget(listWidget)
{}

void FontaFilterEdit::keyPressEvent(QKeyEvent* event)
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

void FontaFilterEdit::mousePressEvent(QMouseEvent * e)
{
    (void)e;
    selectAll();
}

void FontaFilterEdit::suppose(QChar typed)
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

    for(int i = 0; i < listWidget->count(); ++i)
    {
        QListWidgetItem* item = listWidget->item(i);
        CStringRef fontName = item->text();

        if(fontName.startsWith(match, Qt::CaseInsensitive)) {
            setText(fontName);

            ++selectStart;
            setSelection(selectStart, fontName.size()-selectStart);
            break;
        }
    }
}

void FontaFilterEdit::apply()
{
    QList<QListWidgetItem*> items = listWidget->findItems(text(), Qt::MatchExactly);
    if(items.size() > 0) {
        listWidget->setCurrentItem(items[0]);
        listWidget->scrollToItem(items[0], QAbstractItemView::PositionAtCenter);
    }
}

FontaComboBox::FontaComboBox(QWidget* parent)
    : QComboBox(parent)
{
    setLineEdit(new FontaComboBoxLineEdit());
}

void FontaComboBox::wheelEvent(QWheelEvent* e)
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

FontaComboBoxLineEdit::FontaComboBoxLineEdit(QWidget* parent)
    : QLineEdit(parent)
{}

void FontaComboBoxLineEdit::mousePressEvent(QMouseEvent* e)
{
    (void)e;
    selectAll();
}
