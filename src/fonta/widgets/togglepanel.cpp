#include "togglepanel.h"

#include <QStyleOption>
#include <QPainter>

namespace fonta {

TooglePanel::TooglePanel(QWidget* parent)
    : QWidget(parent)
{}


void TooglePanel::toogle(bool toogle)
{
    if(toogle) {
        setStyleSheet(QStringLiteral("background-color:gray;"));
    } else {
        setStyleSheet(QStringLiteral("background-color:white;"));
    }
}

void TooglePanel::paintEvent(QPaintEvent *pe)
{
    (void)pe;

    QStyleOption o;
    o.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(
    QStyle::PE_Widget, &o, &p, this);
}

} // namespace fonta
