#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>

class Canvas : public QWidget
{
    Q_OBJECT
public:
    explicit Canvas(QWidget *parent = 0);

    int d = 232;
    int n = 7;
    int h = 29;
    int w = 92;
    int a = 110;

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // CANVAS_H
