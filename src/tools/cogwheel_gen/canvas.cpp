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
    p.setBrush(QColor(70, 70, 70));
    p.setPen(Qt::NoPen);


    qreal wdth = width();
    qreal hght = height();
    qreal cw = wdth/2.;
    qreal ch = hght/2.;

    qreal r = d/2.;

    p.drawEllipse(cw-r, ch-r, d, d);

    qreal diff = 360/(double)n;
    qreal ang = 0;

    for(int i = 0; i<n; ++i, ang += diff) {
        QPointF pinC = translate(QPointF(cw, ch+r), QPointF(cw, ch), ang);

        QPointF tl = translate(QPointF(pinC.x()-w/2., pinC.y()-h), pinC, ang);
        QPointF tr = translate(QPointF(pinC.x()+w/2., pinC.y()-h), pinC, ang);
        QPointF bl = translate(QPointF(pinC.x()-w/2., pinC.y()+h/**(a/20.)*/), pinC, ang);
        QPointF br = translate(QPointF(pinC.x()+w/2., pinC.y()+h/**(a/20.)*/), pinC, ang);

        bl = translate(bl, tl, -a+90);
        br = translate(br, tr, +a-90);

        QPointF points[4] = { tl, tr, br, bl };

        p.drawPolygon(points, 4);

    }
}
