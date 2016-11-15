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

int callQuestionDialog(CStringRef message);
void callInfoDialog(CStringRef message);

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

class FontaWorkArea;

class RenameTabEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit RenameTabEdit(QTabWidget* tabWidget, FontaWorkArea* workArea, QWidget *parent = 0);
    virtual ~RenameTabEdit(){}

signals:
    void applied();

private slots:
    void keyPressEvent(QKeyEvent* event);
    void focusOutEvent(QFocusEvent* e) { (void)e; deleteLater(); }
private:
    QTabWidget* tabWidget;
    FontaWorkArea* workArea;

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

class QFontDatabase;
class QHBoxLayout;

class FontaField : public QTextEdit
{
    Q_OBJECT

public:
    FontaField(QWidget* parent = 0);
    virtual ~FontaField(){ }

    void toogle(bool toogle) { return m_tooglePanel->toogle(toogle); }
    QWidget* surfaceWidget() { return m_surfaceWidget; }

    int id() const { return m_id; }
    QString fontFamily() const { return font().family(); }
    float fontSize() const { return font().pointSizeF(); }
    QString fontStyle() const { return m_fontStyle; }
    QString preferableFontStyle() const { return m_preferableFontStyle; }
    Qt::Alignment textAlignment() const { return m_alignment; }
    float leading() const { return m_leading; }
    int tracking() const { return m_tracking; }

    void setId(int id) { m_id = id; }
    void setFontFamily(CStringRef family);
    void setFontSize(float size);
    void setFontStyle(CStringRef style);
    void setPreferableFontStyle(CStringRef style);
    void alignText(Qt::Alignment alignment);
    void setLeading(float val);
    void setTracking(int val);
    StyleSheet& sheet() const { return m_sheet; }
    void applySheet() { setStyleSheet(m_sheet.get()); }
    void setSamples(CStringRef latin, CStringRef rus) { m_latinText = latin; m_rusText = rus; }

    void save(QJsonObject &json) const;
    void load(const QJsonObject &json);

    static constexpr bool showBorders = false;

signals:
    void focussed(FontaField* field);

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

class FontaWorkArea : public QSplitter {

    Q_OBJECT

public:
    FontaWorkArea(int id, QWidget* parent, QString name = "");
    virtual ~FontaWorkArea();

    friend class Sampler;

    void createSample();

    int id() const { return m_id; }
    void setId(int id) { m_id = id; }
    CStringRef name() const { return m_name; }
    void rename(CStringRef name) { m_name = name; }
    FontaField* currField() const { return m_currField; }
    void setCurrField(FontaField* field) { m_currField = field; }

    FontaField* addField();
    void popField();
    FontaField* operator[](int i);
    int fieldCount() const { return m_fields.size(); }
    void clear();

    void save(QJsonObject &json) const;
    void load(const QJsonObject &json);

private:
    void loadSample(CStringRef jsonTxt);

private slots:
    void on_currentFieldChanged(FontaField* field);

private:
    int m_id;
    QString m_name;
    QVector<FontaField*> m_fields;
    FontaField* m_currField;
};

class QListWidget;

class FontaFilterEdit : public QLineEdit
{
    Q_OBJECT

public:
    FontaFilterEdit(QListWidget* listWidget, QWidget* parent = 0);
    virtual ~FontaFilterEdit(){}

protected:
    void keyPressEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent * e);

private:
    QListWidget* listWidget;

    void apply();
    void suppose(QChar typed);
};

class FontaComboBoxLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    FontaComboBoxLineEdit(QWidget* parent = 0);
    virtual ~FontaComboBoxLineEdit(){}

protected:
    void mousePressEvent(QMouseEvent* e);
};

class FontaComboBox : public QComboBox
{
    Q_OBJECT

public:
    FontaComboBox(QWidget* parent = 0);
    virtual ~FontaComboBox(){}

protected:
    void wheelEvent(QWheelEvent* e);
    //void focusOutEvent(QFocusEvent* e){(void)e;}
};

#endif // FONTAWIDGETS_H
