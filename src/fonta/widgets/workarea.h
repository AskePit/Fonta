#ifndef WORKAREA_H
#define WORKAREA_H

#include "types.h"
#include "field.h"
#include <QSplitter>

namespace fonta {

class WorkArea : public QSplitter {

    Q_OBJECT

public:
    WorkArea(int id, QWidget* parent, QString name = "");
    virtual ~WorkArea();

    friend class Sampler;

    void createSample();

    int id() const;
    void setId(int id);
    CStringRef name() const;
    void rename(CStringRef name);
    Field* currField() const;
    void setCurrField(Field* field);

    Field* addField(InitType initType = InitType::Sampled);
    Field* addField(Field* field);
    void popField();
    Field* operator[](int i);
    int fieldCount() const;
    QVector<Field*>::iterator begin();
    QVector<Field*>::const_iterator begin() const;
    QVector<Field*>::iterator end();
    QVector<Field*>::const_iterator end() const;

    void clear();

    void save(QJsonObject &json) const;
    void load(const QJsonObject &json);

private:
    void loadSample(CStringRef jsonTxt);

private slots:
    void on_currentFieldChanged();

private:
    int m_id;
    QString m_name;
    QVector<Field*> m_fields;
    Field* m_currField;
};

} // namespace fonta

#endif // WORKAREA_H
