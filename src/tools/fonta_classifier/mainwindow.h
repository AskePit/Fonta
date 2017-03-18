#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "types.h"
#include "classifier.h"

#include <QMainWindow>
#include <QCheckBox>
#include <QSpinBox>
#include <QSet>
#include <QMap>
#include <QSettings>
#include <functional>

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
    void keyPressEvent(QKeyEvent *event);

private slots:
    void on_actionOpen_triggered();
    void on_saveButton_clicked();

private:
    Ui::MainWindow *ui;

    QMap<FontType::type, QSet<QString>> m_db;
    QString m_dbPath;
    QSettings m_reg;

    void connectBoxes();

    bool loadDB();
    void storeDB();

    void doCheckboxes(std::function<void(QCheckBox *, bool)> func, bool b);

    void clearUi();
    void onLoadSuccess();
    void onLoadFailure();

    bool loadFontType(FontType::type t);

    void search();
};

#endif // MAINWINDOW_H
