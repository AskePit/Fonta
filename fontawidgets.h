#ifndef FONTAWIDGETS_H
#define FONTAWIDGETS_H

#include "types.h"
#include <QTextEdit>
#include <QLineEdit>
#include <QComboBox>
#include <QSplitter>
#include <QDialog>
#include "stylesheet.h"

class QVBoxLayout;
class QHBoxLayout;
class QSpacerItem;
class QLabel;

class QFontDatabase;
class QHBoxLayout;

class QListWidget;

int callQuestionDialog(CStringRef message);
void callInfoDialog(CStringRef message);

namespace fonta {

class About : public QDialog
{
    Q_OBJECT

public:
    explicit About(const Version& version, QWidget *parent = 0);
    ~About();

private slots:
    void on_pushButton_clicked();

private:
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer_3;
    QLabel *label;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer;
};

class WorkArea;

class RenameTabEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit RenameTabEdit(QTabWidget* tabWidget, WorkArea* workArea, QWidget *parent = 0);
    virtual ~RenameTabEdit(){}

signals:
    void applied();

private slots:
    void keyPressEvent(QKeyEvent* event);
    void focusOutEvent(QFocusEvent* e) { (void)e; deleteLater(); }
private:
    QTabWidget* m_tabWidget;
    WorkArea* m_workArea;

    void apply();
};

class TooglePanel : public QWidget
{
    Q_OBJECT

public:
    TooglePanel(QWidget* parent = 0);
    virtual ~TooglePanel(){}

public slots:
    void toogle(bool toogle);

protected:
    void paintEvent(QPaintEvent *pe);
};

class Field : public QTextEdit
{
    Q_OBJECT

public:
    Field(InitType initType = InitType::Sampled, QWidget* parent = 0);
    virtual ~Field(){ }

    void toogle(bool toogle);
    QWidget* surfaceWidget();

    int id() const;
    QString fontFamily() const;
    float fontSize() const;
    QString fontStyle() const;
    QString preferableFontStyle() const;
    Qt::Alignment textAlignment() const;
    float leading() const;
    int tracking() const;

    void setId(int id);
    void setFontFamily(CStringRef family);
    void setFontSize(float size);
    void setFontStyle(CStringRef style);
    void setPreferableFontStyle(CStringRef style);
    void alignText(Qt::Alignment alignment);
    void setLeading(float val);
    void setTracking(int val);
    StyleSheet& sheet() const;
    void applySheet();
    void setSamples(CStringRef latin, CStringRef rus);

    void save(QJsonObject &json) const;
    void load(const QJsonObject &json);

    static constexpr bool showBorders = false;

signals:
    void focussed(Field* field);

protected:
    void focusInEvent(QFocusEvent* e);
    void keyPressEvent(QKeyEvent *k);

private:
    QString m_fontStyle;
    QString m_preferableFontStyle;
    int m_id;
    Qt::Alignment m_alignment;
    float m_leading;
    int m_tracking;
    mutable StyleSheet m_sheet;

    QString m_latinText;
    QString m_rusText;
    bool m_userChangedText;

    QWidget* m_surfaceWidget;
    QHBoxLayout* m_surfaceLayout;
    TooglePanel* m_tooglePanel;

    void alignTextHorizontally(Qt::Alignment alignment);
};

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
    void popField();
    Field* operator[](int i);
    int fieldCount() const;
    void clear();

    void save(QJsonObject &json) const;
    void load(const QJsonObject &json);

private:
    void loadSample(CStringRef jsonTxt);

private slots:
    void on_currentFieldChanged(Field* field);

private:
    int m_id;
    QString m_name;
    QVector<Field*> m_fields;
    Field* m_currField;
};

class FilterEdit : public QLineEdit
{
    Q_OBJECT

public:
    FilterEdit(QListWidget* listWidget, QWidget* parent = 0);
    virtual ~FilterEdit(){}

protected:
    void keyPressEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent * e);

private:
    QListWidget* listWidget;

    void apply();
    void suppose(QChar typed);
};

class ComboBoxLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    ComboBoxLineEdit(QWidget* parent = 0);
    virtual ~ComboBoxLineEdit(){}

protected:
    void mousePressEvent(QMouseEvent* e);
};

class ComboBox : public QComboBox
{
    Q_OBJECT

public:
    ComboBox(QWidget* parent = 0);
    virtual ~ComboBox(){}

protected:
    void wheelEvent(QWheelEvent* e);
    //void focusOutEvent(QFocusEvent* e){(void)e;}
};

} // namespace fonta

#endif // FONTAWIDGETS_H
