#ifndef FIELD_H
#define FIELD_H

#include "types_fonta.h"
#include "stylesheet.h"
#include <QTextEdit>

class QHBoxLayout;

namespace fonta {

class WorkArea;
class TooglePanel;

class Field : public QTextEdit
{
    Q_OBJECT

public:
    Field(InitType initType = InitType::Sampled, QWidget* parent = 0);
    virtual ~Field(){ }

    Field& operator=(const Field &other);
    Field* clone() const;

    void toogle(bool toogle);
    QWidget* surfaceWidget();

    void swapFamiliesWith(Field *other);

    int id() const;
    QString fontFamily() const;
    float fontSize() const;
    QString fontStyle() const;
    QString preferableFontStyle() const;
    Qt::Alignment textAlignment() const;
    float leading() const;
    int tracking() const;
    ContentMode contentMode();
    LanguageContext languageContext();

    void setId(int id);
    void setFontFamily(CStringRef family);
    void setFontSize(float size);
    void setFontStyle(CStringRef style);
    void setPreferableFontStyle(CStringRef style);
    void alignText(Qt::Alignment alignment);
    void setLeading(float val);
    void setTracking(int val);
    void setContentMode(ContentMode mode);
    void setLanguageContext(LanguageContext context);

    StyleSheet& sheet() const;
    void applySheet();
    void fetchSamples();
    void updateText();
    void updateLoremText();

    void save(QJsonObject &json) const;
    void load(const QJsonObject &json);

signals:
    void focussed();
    void contentBecameUserDefined();
    void swapRequested();

public slots:
    void showContextMenu(const QPoint &point);

protected:
    void focusInEvent(QFocusEvent* e);
    void keyPressEvent(QKeyEvent *k);
    void resizeEvent(QResizeEvent* e);
    void timerEvent(QTimerEvent* e);

private:
    QString m_fontStyle;
    QString m_preferableFontStyle;
    int m_id;
    Qt::Alignment m_alignment;
    float m_leading;
    int m_tracking;
    mutable StyleSheet m_sheet;

    QString m_engText;
    QString m_rusText;

    ContentMode m_contentMode;
    LanguageContext m_languageContext;

    int m_timerId;

    QWidget* m_surfaceWidget;
    QHBoxLayout* m_surfaceLayout;
    TooglePanel* m_tooglePanel;

    void updateLeading();
    void alignTextHorizontally(Qt::Alignment alignment);
};

} // namespace fonta

#endif // FIELD_H
