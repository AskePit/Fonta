#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>

class Canvas : public QWidget
{
    Q_OBJECT
public:
    explicit Canvas(QWidget *parent = 0);

    int d = 100;
    int n = 6;
    int h = 10;
    int w = 10;
    int a = 90;

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // CANVAS_H
