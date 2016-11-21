#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCheckBox>

class TristBox : public QCheckBox {
        Q_OBJECT

public:
    TristBox(QWidget *parent = 0) : QCheckBox(parent)
    {
        setTristate(true);
        connect(this, &QCheckBox::clicked, this, &TristBox::processToogle);
    }

protected:
    void processToogle() {
        switch(checkState()) {
            case Qt::Unchecked: setCheckState(Qt::PartiallyChecked); return;
            case Qt::Checked: setCheckState(Qt::Unchecked); return;
            case Qt::PartiallyChecked: setCheckState(Qt::Checked); return;
        }
    }
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
