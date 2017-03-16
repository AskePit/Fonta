#ifndef FILTEREDIT_H
#define FILTEREDIT_H

#include <QLineEdit>

class QListWidget;

namespace fonta {

class FilterEdit : public QLineEdit
{
    Q_OBJECT

public:
    FilterEdit(QWidget* parent = 0);
    void setListWidget(QListWidget* listWidget) { m_listWidget = listWidget; }
    virtual ~FilterEdit(){}

protected:
    void keyPressEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent * e);

private:
    QListWidget* m_listWidget;

    void apply();
    void suppose(QChar typed);
};

} // namespace fonta

#endif // FILTEREDIT_H
