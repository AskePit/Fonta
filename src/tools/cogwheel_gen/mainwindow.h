#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_dBox_valueChanged(int arg1);

    void on_nBox_valueChanged(int arg1);

    void on_hBox_valueChanged(int arg1);

    void on_wBox_valueChanged(int arg1);

    void on_aBox_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
