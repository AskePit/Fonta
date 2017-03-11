#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCheckBox>
#include <QSpinBox>

namespace Ui {
class MainWindow;
}

class Info;

enum Direction {
    Forward,
    Backward,
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent* event);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void on_actionOpen_triggered();
    void on_saveButton_clicked();

private:
    Ui::MainWindow *ui;

    void openFile(const QString &filename);

    void readConfig();
    void saveConfig();
};

#endif // MAINWINDOW_H
