#ifndef INFO_H
#define INFO_H

#include <QDialog>

namespace Ui {
class Info;
}

class Info : public QDialog
{
    Q_OBJECT

public:
    explicit Info(QWidget *parent = 0);
    ~Info();

    friend class MainWindow;

private:
    Ui::Info *ui;
};

#endif // INFO_H
