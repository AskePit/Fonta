#include "filteredit.h"
#include "types.h"

#include <QKeyEvent>
#include <QListWidget>

namespace fonta {

FilterEdit::FilterEdit(QWidget* parent)
    : QLineEdit(parent)
{}

void FilterEdit::keyPressEvent(QKeyEvent* event)
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

void FilterEdit::mousePressEvent(QMouseEvent * e)
{
    (void)e;
    selectAll();
}

void FilterEdit::suppose(QChar typed)
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

    for(int i = 0; i < m_listWidget->count(); ++i)
    {
        QListWidgetItem* item = m_listWidget->item(i);
        CStringRef fontName = item->text();

        if(fontName.startsWith(match, Qt::CaseInsensitive)) {
            setText(fontName);

            ++selectStart;
            setSelection(selectStart, fontName.size()-selectStart);
            break;
        }
    }
}

void FilterEdit::apply()
{
    QList<QListWidgetItem*> items = m_listWidget->findItems(text(), Qt::MatchExactly);
    if(items.size() > 0) {
        m_listWidget->setCurrentItem(items[0]);
        m_listWidget->scrollToItem(items[0], QAbstractItemView::PositionAtCenter);
    }
}

} // namespace fonta
