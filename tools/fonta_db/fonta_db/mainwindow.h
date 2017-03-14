#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

// some types definitions
using CStringRef = const QString&;
#define enum_class(x) class x { public: enum type
#define enum_interface };
#define enum_end ;}

enum_class (FontType) {
    Start = 0,
    Serif = Start,
    Sans,
    Script,
    Display,
    Symbolic,
    Oldstyle,
    Transitional,
    Modern,
    Slab,
    Grotesque,
    Geometric,
    Humanist,
    Monospaced,
    End

enum_interface
    static QString fileName(FontType::type t) {
        switch(t) {
            case FontType::Serif: return "serif.dat";
            case FontType::Sans: return "sans.dat";
            case FontType::Script: return "script.dat";
            case FontType::Display: return "decorative.dat";
            case FontType::Symbolic: return "symbolic.dat";
            case FontType::Oldstyle: return "old_style.dat";
            case FontType::Transitional: return "transitional.dat";
            case FontType::Modern: return "modern.dat";
            case FontType::Slab: return "slab.dat";
            case FontType::Grotesque: return "grotesque.dat";
            case FontType::Geometric: return "geometric.dat";
            case FontType::Humanist: return "humanist.dat";
            case FontType::Monospaced: return "monospaced.dat";
            default: return "";
        }
    }
};

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
