#include "combobox.h"
#include "types.h"

#include <QWheelEvent>

namespace fonta {

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
