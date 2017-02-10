#include "workarea.h"

#include "sampler.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

namespace fonta {

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
    return addField(field);
}

Field* WorkArea::addField(Field* field)
{
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

QVector<Field*>::iterator WorkArea::begin()
{
    return m_fields.begin();
}

QVector<Field*>::const_iterator WorkArea::begin() const
{
    return m_fields.begin();
}

QVector<Field*>::iterator WorkArea::end()
{
    return m_fields.end();
}

QVector<Field*>::const_iterator WorkArea::end() const
{
    return m_fields.end();
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

} // namespace fonta
