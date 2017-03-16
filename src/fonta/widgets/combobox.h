#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QLineEdit>
#include <QComboBox>

namespace fonta {

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

#endif // COMBOBOX_H
