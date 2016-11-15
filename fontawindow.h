#ifndef FONTAWINDOW_H
#define FONTAWINDOW_H

#include "fontadb.h"
#include <QMainWindow>
#include <QVector>
#include "types.h"

namespace Ui {
class FontaWindow;
}

class QPushButton;
class QButtonGroup;
class FontaWorkArea;
class FontaField;
class FontaFilterEdit;
class About;

enum_class (FilterMode) {
    Start,
    ALL = Start,
    CYRILLIC,
    SERIF,
    SANS_SERIF,
    MONOSPACE,
    SCRIPT,
    DECORATIVE,
    SYMBOLIC,
    End

    enum_interface
    static QString toString(type t) {
        switch(t) {
        default:
        case ALL:           return "[All]";         break;
        case CYRILLIC:      return "Cyrillic";      break;
        case SERIF:         return "Serif";         break;
        case SANS_SERIF:    return "Sans Serif";    break;
        case MONOSPACE:     return "Monospace";     break;
        case SCRIPT:        return "Script";        break;
        case DECORATIVE:    return "Decorative";    break;
        case SYMBOLIC:      return "Symbolic";      break;
        }
    }
};

class FontaWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FontaWindow(CStringRef fileToOpen = QString(), QWidget *parent = 0);
    ~FontaWindow();

    static const Version versionNumber;

    void filterFontList(const QStringList& l);

private slots:
    void on_fontsList_currentTextChanged(const QString &currentText);
    void on_addFieldButton_clicked();
    void on_removeFieldButton_clicked();
    void on_currentFieldChanged(FontaField* textEdit);

    void on_sizeBox_edited();
    void on_sizeBox_activated(const QString &arg1);
    void on_leadingBox_edited();
    void on_leadingBox_activated(const QString &arg1);
    void on_trackingBox_edited();
    void on_trackingBox_activated(const QString &arg1);
    void on_filterBox_currentIndexChanged(int index);
    void on_styleBox_activated(const QString &arg1);

    void showTabsContextMenu(const QPoint &point);
    void addTab(bool empty = false);
    void closeTabPrompted(int i);
    void closeTab(int i);
    void closeOtherTabs();
    void renameTab(int id);

    void showFontListContextMenu(const QPoint &point);
    void uninstallFont(const QString &fontName);

    void on_actionSave_as_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionNew_triggered();

    void on_tabWidget_currentChanged(int index);
    void changeAddTabButtonGeometry();
    void on_actionAbout_triggered();
    void onTabsMove(int, int);
    void on_filterWizardButton_clicked();

    void on_backColorButton_clicked();
    void on_textColorButton_clicked();

    void on_actionNew_Tab_triggered();
    void on_actionClose_Tab_triggered();
    void on_actionClose_other_Tabs_triggered();
    void on_actionNext_Tab_triggered();

    void on_actionFonts_Cleaner_triggered();

protected:
    void resizeEvent(QResizeEvent* event);

private:
    Ui::FontaWindow *ui;
    About* aboutDialog;

    QVector<FontaWorkArea*> workAreas;
    FontaWorkArea* currWorkArea;
    FontaField* currField;
    FontaFilterEdit* fontFinderEdit;

    // alignment
    QPushButton *topLeftButton;
    QPushButton *topCenterButton;
    QPushButton *topRightButton;
    QPushButton *topJustifyButton;
    QButtonGroup* alignButtosGroup;

    QPushButton *addTabButton;

    void initAlignButton(QPushButton*& button, CStringRef iconPath, Qt::Alignment alignment);

    void saveGeometry();
    void loadGeometry();

    void save(CStringRef fileName) const;
    void load(CStringRef fileName);

    void openFile(CStringRef fileName);

    void clearWorkAreas();

    QString currentProjectFile;
    void setCurrentProjectFile(CStringRef filename);
    void resetCurrentProjectFile();

    void makeFieldConnected(FontaField* textEdit);
    void makeFieldsConnected();

    void setCurrWorkArea(int id);
    void updateAddRemoveButtons();
};

#endif // FONTAWINDOW_H
