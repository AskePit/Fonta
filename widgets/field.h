#ifndef FIELD_H
#define FIELD_H

#include "types.h"
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

    void save(QJsonObject &json) const;
    void load(const QJsonObject &json);

signals:
    void focussed(Field* field);
    void contentBecameUserDefined();

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
    bool m_leadingChanged;
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

    void alignTextHorizontally(Qt::Alignment alignment);
};

} // namespace fonta

#endif // FIELD_H
