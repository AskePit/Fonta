#include "canvas.h"
#include <QPainter>
#include <QDebug>

Canvas::Canvas(QWidget *parent) : QWidget(parent)
{
    setAutoFillBackground(true);
}

constexpr auto pi = 3.14159265358979323846;

inline static double rad(double degree) {
    return degree*pi/180;
}

static QPointF translate(QPointF point, const QPointF &center, qreal ang)
{
    qreal rad_ang = rad(ang);

    point -= center;
    qreal x = point.x();
    qreal y = point.y();

    qreal newX = x*cos(rad_ang) - y*sin(rad_ang);
    qreal newY = y*cos(rad_ang) + x*sin(rad_ang);

    point.setX(newX);
    point.setY(newY);

    point += center;
    return point;
}

void Canvas::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    //p.setBrush(QColor(70, 70, 70));
    //p.setPen(Qt::NoPen);


    int wdth = width();
    int hght = height();
    int cw = wdth/2;
    int ch = hght/2;

    int r = d/2;

    p.drawEllipse(cw-r, ch-r, d, d);

    qreal diff = 360/(double)n;
    qreal ang = 0;
    for(int i = 0; i<n; ++i, ang += diff) {
        p.setPen(QPen(QBrush(QColor(70, 70, 70)), 5));
        p.drawPoint(translate(QPointF(cw, ch-r), QPointF(cw, ch), ang));
    }
}
