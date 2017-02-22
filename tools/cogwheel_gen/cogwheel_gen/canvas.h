#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>

class Canvas : public QWidget
{
    Q_OBJECT
public:
    explicit Canvas(QWidget *parent = 0);

    int d = 250;
    int n = 5;
    int h = 20;
    int w = 95;
    int a = 110;

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // CANVAS_H
