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
    static const QMap<fonta::FontType::type, QCheckBox *> m_boxesMap;

    QString m_dbPath;
    bool m_loaded {false};
    fonta::Classifier m_classifier;
    bool m_found {false};

    QSettings m_reg;

    void connectBoxes();

    bool loadDB();

    void doCheckboxes(std::function<void(QCheckBox *, bool)> func, bool b);

    void clearUi();
    void onLoadSuccess();
    void onLoadFailure();

    void search();
};

#endif // MAINWINDOW_H
