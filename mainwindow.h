#ifndef FONTAWINDOW_H
#define FONTAWINDOW_H

#include "fontadb.h"
#include <QMainWindow>
#include <QVector>
#include "types.h"

namespace Ui {
class MainWindow;
}

class QPushButton;
class QActionGroup;
class QButtonGroup;

namespace fonta {

class WorkArea;
class Field;
class FilterEdit;
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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(CStringRef fileToOpen = QString(), QWidget *parent = 0);
    ~MainWindow();

    static const Version versionNumber;

    void filterFontList(const QStringList& l);

private slots:
    void on_fontsList_currentTextChanged(const QString &currentText);
    void on_addFieldButton_clicked();
    void on_removeFieldButton_clicked();
    void on_currentFieldChanged();
    void updateFontFamily();

    void on_sizeBox_edited();
    void on_sizeBox_activated(const QString &arg1);
    void on_leadingBox_edited();
    void on_leadingBox_activated(const QString &arg1);
    void on_trackingBox_edited();
    void on_trackingBox_activated(const QString &arg1);
    void on_filterBox_currentIndexChanged(int index);
    void on_styleBox_activated(const QString &arg1);

    void showTabsContextMenu(const QPoint &point);
    void addTab(InitType initType = InitType::Sampled);
    void closeTabPrompted(int i);
    void closeTab(int i);
    void closeOtherTabs();
    void cloneCurrTab();
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

    void on_alignLeftButton_toggled();
    void on_alignCenterButton_toggled();
    void on_alignRightButton_toggled();
    void on_alignJustifyButton_toggled();

    void on_actionFillNews_triggered();
    void on_actionFillPangram_triggered();
    void on_actionFillLoremIpsum_triggered();
    void resetFillActions();

    void updateContextGroup();
    void enableContextGroup();
    void disableContextGroup();

    void swapFonts();

protected:
    void resizeEvent(QResizeEvent* event);

private:
    Ui::MainWindow *ui;
    About* m_aboutDialog;

    QVector<WorkArea*> m_workAreas;
    WorkArea* m_currWorkArea;
    Field* m_currField;

    QPushButton *m_addTabButton;

    QActionGroup *fillGroup;

    QButtonGroup *contextGroup;
    QPushButton *autoButton;
    QPushButton *engButton;
    QPushButton *rusButton;

    void saveGeometry();
    void loadGeometry();

    void save(CStringRef fileName) const;
    void load(CStringRef fileName);

    void openFile(CStringRef fileName);

    void clearWorkAreas();

    QString m_currFile;
    void setCurrFile(CStringRef filename);
    void resetCurrFile();

    void makeFieldConnected(Field* textEdit);
    void makeFieldsConnected();

    void setCurrWorkArea(int id);
    void updateAddRemoveButtons();

    void extendToolBar();

    Field *m_swapRequester;
    void swapBlockState(bool enable);
};

} // namespace fonta

#endif // FONTAWINDOW_H
