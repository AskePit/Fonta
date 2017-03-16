#ifndef TOGGLEPANEL_H
#define TOGGLEPANEL_H

#include <QWidget>

namespace fonta {

class TooglePanel : public QWidget
{
    Q_OBJECT

public:
    TooglePanel(QWidget* parent = 0);
    virtual ~TooglePanel(){}

public slots:
    void toogle(bool toogle);

protected:
    void paintEvent(QPaintEvent *pe);
};

} // namespace fonta

#endif // TOGGLEPANEL_H
