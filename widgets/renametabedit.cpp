#include "renametabedit.h"
#include "workarea.h"

#include <QTabWidget>
#include <QKeyEvent>
#include <QTabBar>

namespace fonta {

RenameTabEdit::RenameTabEdit(QTabWidget* tabWidget, WorkArea* workArea, QWidget *parent)
    : QLineEdit(parent)
    , m_tabWidget(tabWidget)
    , m_workArea(workArea)
{
    setWindowFlags(Qt::FramelessWindowHint);

    const QRect& r = tabWidget->tabBar()->tabRect(workArea->id());
    setGeometry(r);
    setText(workArea->name());
    selectAll();
    setFocus();
}

void RenameTabEdit::keyPressEvent(QKeyEvent* event)
{
    int key = event->key();
    if(key == Qt::Key_Return) {
        apply();
    } else {
        QLineEdit::keyPressEvent(event);
    }
}

void RenameTabEdit::apply()
{
    QString txt = text();
    m_workArea->rename(txt);
    m_tabWidget->setTabText(m_workArea->id(), txt);
    emit applied();
    deleteLater();
}

} // namespace fonta
