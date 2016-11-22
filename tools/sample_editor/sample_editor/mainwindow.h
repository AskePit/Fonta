#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCheckBox>
#include <QSpinBox>

/*
 * CheckBox with special states order:
 * unchecked -> checked -> semistate
 */
class TristBox : public QCheckBox {
        Q_OBJECT

public:
    TristBox(QWidget *parent = 0)
        : QCheckBox(parent)
    {
        setTristate(true);
        connect(this, &QCheckBox::clicked, [this](){
            switch(checkState()) {
                case Qt::Unchecked: setCheckState(Qt::PartiallyChecked); return;
                case Qt::Checked: setCheckState(Qt::Unchecked); return;
                case Qt::PartiallyChecked: setCheckState(Qt::Checked); return;
            }
        });
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

protected:
    void resizeEvent(QResizeEvent* event);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void on_actionOpen_triggered();

    void on_nextButton_clicked();

    void on_backButton_clicked();

private:
    Ui::MainWindow *ui;

    QStringList files;
    int pos;

    void openDir(const QString &dirName);
    void nextSample();
    void prevSample();
};

#endif // MAINWINDOW_H
