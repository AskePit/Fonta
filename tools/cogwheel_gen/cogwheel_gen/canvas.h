#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>

class Canvas : public QWidget
{
    Q_OBJECT
public:
    explicit Canvas(QWidget *parent = 0);

    int d = 250;
    int n = 7;
    int h = 44;
    int w = 100;
    int a = 110;

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // CANVAS_H
