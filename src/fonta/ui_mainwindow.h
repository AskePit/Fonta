/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <widgets/filteredit.h>
#include "widgets/combobox.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionSave_as;
    QAction *actionOpen;
    QAction *actionSave;
    QAction *actionNew;
    QAction *actionAbout;
    QAction *actionNew_Tab;
    QAction *actionClose_Tab;
    QAction *actionNext_Tab;
    QAction *actionPrevious_Tab;
    QAction *actionClose_other_Tabs;
    QAction *actionFonts_Cleaner;
    QAction *actionFillNews;
    QAction *actionFillPangram;
    QAction *actionFillLoremIpsum;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QSplitter *fontsListSplitter;
    QWidget *fontsListLayoutWidget;
    QVBoxLayout *fontsListLayout;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *filterWizardButton;
    QComboBox *filterBox;
    fonta::FilterEdit *fontFinderEdit;
    QListWidget *fontsList;
    QWidget *workAreaLayoutWidget;
    QVBoxLayout *workAreaLayout;
    QTabWidget *tabWidget;
    QWidget *bottomWidget;
    QHBoxLayout *horizontalLayout;
    QWidget *widget;
    QLabel *sizeLabel;
    fonta::ComboBox *sizeBox;
    QLabel *styleLabel;
    QComboBox *styleBox;
    QLabel *leadingLabel;
    fonta::ComboBox *leadingBox;
    QLabel *trackingLabel;
    fonta::ComboBox *trackingBox;
    QFrame *line;
    QPushButton *alignLeftButton;
    QPushButton *alignCenterButton;
    QPushButton *alignRightButton;
    QPushButton *alignJustifyButton;
    QFrame *line_2;
    QPushButton *textColorButton;
    QPushButton *backColorButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *addFieldButton;
    QPushButton *removeFieldButton;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QMenu *menuTabs;
    QStatusBar *statusBar;
    QToolBar *toolBar;
    QButtonGroup *alignButtonGroup;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->setEnabled(true);
        MainWindow->resize(830, 492);
        actionSave_as = new QAction(MainWindow);
        actionSave_as->setObjectName(QStringLiteral("actionSave_as"));
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        actionNew = new QAction(MainWindow);
        actionNew->setObjectName(QStringLiteral("actionNew"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        actionNew_Tab = new QAction(MainWindow);
        actionNew_Tab->setObjectName(QStringLiteral("actionNew_Tab"));
        actionClose_Tab = new QAction(MainWindow);
        actionClose_Tab->setObjectName(QStringLiteral("actionClose_Tab"));
        actionClose_Tab->setEnabled(false);
        actionNext_Tab = new QAction(MainWindow);
        actionNext_Tab->setObjectName(QStringLiteral("actionNext_Tab"));
        actionNext_Tab->setEnabled(false);
        actionPrevious_Tab = new QAction(MainWindow);
        actionPrevious_Tab->setObjectName(QStringLiteral("actionPrevious_Tab"));
        actionPrevious_Tab->setEnabled(false);
        actionClose_other_Tabs = new QAction(MainWindow);
        actionClose_other_Tabs->setObjectName(QStringLiteral("actionClose_other_Tabs"));
        actionClose_other_Tabs->setEnabled(false);
        actionFonts_Cleaner = new QAction(MainWindow);
        actionFonts_Cleaner->setObjectName(QStringLiteral("actionFonts_Cleaner"));
        actionFillNews = new QAction(MainWindow);
        actionFillNews->setObjectName(QStringLiteral("actionFillNews"));
        actionFillNews->setCheckable(true);
        QIcon icon;
        icon.addFile(QStringLiteral(":/pic/rss.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionFillNews->setIcon(icon);
        actionFillPangram = new QAction(MainWindow);
        actionFillPangram->setObjectName(QStringLiteral("actionFillPangram"));
        actionFillPangram->setCheckable(true);
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/pic/fox.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionFillPangram->setIcon(icon1);
        actionFillLoremIpsum = new QAction(MainWindow);
        actionFillLoremIpsum->setObjectName(QStringLiteral("actionFillLoremIpsum"));
        actionFillLoremIpsum->setCheckable(true);
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/pic/lorem.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionFillLoremIpsum->setIcon(icon2);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        fontsListSplitter = new QSplitter(centralWidget);
        fontsListSplitter->setObjectName(QStringLiteral("fontsListSplitter"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(fontsListSplitter->sizePolicy().hasHeightForWidth());
        fontsListSplitter->setSizePolicy(sizePolicy);
        fontsListSplitter->setOrientation(Qt::Horizontal);
        fontsListSplitter->setHandleWidth(0);
        fontsListLayoutWidget = new QWidget(fontsListSplitter);
        fontsListLayoutWidget->setObjectName(QStringLiteral("fontsListLayoutWidget"));
        fontsListLayout = new QVBoxLayout(fontsListLayoutWidget);
        fontsListLayout->setSpacing(6);
        fontsListLayout->setContentsMargins(11, 11, 11, 11);
        fontsListLayout->setObjectName(QStringLiteral("fontsListLayout"));
        fontsListLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(3);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setSizeConstraint(QLayout::SetDefaultConstraint);
        filterWizardButton = new QPushButton(fontsListLayoutWidget);
        filterWizardButton->setObjectName(QStringLiteral("filterWizardButton"));
        filterWizardButton->setMinimumSize(QSize(22, 22));
        filterWizardButton->setMaximumSize(QSize(22, 22));

        horizontalLayout_3->addWidget(filterWizardButton);

        filterBox = new QComboBox(fontsListLayoutWidget);
        filterBox->setObjectName(QStringLiteral("filterBox"));
        filterBox->setEditable(false);

        horizontalLayout_3->addWidget(filterBox);


        fontsListLayout->addLayout(horizontalLayout_3);

        fontFinderEdit = new fonta::FilterEdit(fontsListLayoutWidget);
        fontFinderEdit->setObjectName(QStringLiteral("fontFinderEdit"));

        fontsListLayout->addWidget(fontFinderEdit);

        fontsList = new QListWidget(fontsListLayoutWidget);
        fontsList->setObjectName(QStringLiteral("fontsList"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(fontsList->sizePolicy().hasHeightForWidth());
        fontsList->setSizePolicy(sizePolicy1);

        fontsListLayout->addWidget(fontsList);

        fontsListSplitter->addWidget(fontsListLayoutWidget);
        workAreaLayoutWidget = new QWidget(fontsListSplitter);
        workAreaLayoutWidget->setObjectName(QStringLiteral("workAreaLayoutWidget"));
        workAreaLayout = new QVBoxLayout(workAreaLayoutWidget);
        workAreaLayout->setSpacing(6);
        workAreaLayout->setContentsMargins(11, 11, 11, 11);
        workAreaLayout->setObjectName(QStringLiteral("workAreaLayout"));
        workAreaLayout->setContentsMargins(5, 0, 0, 0);
        tabWidget = new QTabWidget(workAreaLayoutWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setTabPosition(QTabWidget::North);
        tabWidget->setTabShape(QTabWidget::Rounded);
        tabWidget->setElideMode(Qt::ElideNone);
        tabWidget->setDocumentMode(true);
        tabWidget->setTabsClosable(true);
        tabWidget->setMovable(true);
        tabWidget->setTabBarAutoHide(false);

        workAreaLayout->addWidget(tabWidget);

        bottomWidget = new QWidget(workAreaLayoutWidget);
        bottomWidget->setObjectName(QStringLiteral("bottomWidget"));
        bottomWidget->setMinimumSize(QSize(0, 65));
        horizontalLayout = new QHBoxLayout(bottomWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, -1, -1, 0);
        widget = new QWidget(bottomWidget);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setMinimumSize(QSize(250, 0));
        sizeLabel = new QLabel(widget);
        sizeLabel->setObjectName(QStringLiteral("sizeLabel"));
        sizeLabel->setGeometry(QRect(0, 2, 28, 25));
        sizeBox = new fonta::ComboBox(widget);
        sizeBox->setObjectName(QStringLiteral("sizeBox"));
        sizeBox->setGeometry(QRect(30, 5, 105, 20));
        sizeBox->setMinimumSize(QSize(55, 0));
        sizeBox->setMaximumSize(QSize(200, 16777215));
        sizeBox->setEditable(true);
        sizeBox->setMaxVisibleItems(13);
        sizeBox->setInsertPolicy(QComboBox::NoInsert);
        styleLabel = new QLabel(widget);
        styleLabel->setObjectName(QStringLiteral("styleLabel"));
        styleLabel->setGeometry(QRect(0, 27, 28, 25));
        styleBox = new QComboBox(widget);
        styleBox->setObjectName(QStringLiteral("styleBox"));
        styleBox->setGeometry(QRect(30, 30, 105, 20));
        styleBox->setMinimumSize(QSize(55, 0));
        leadingLabel = new QLabel(widget);
        leadingLabel->setObjectName(QStringLiteral("leadingLabel"));
        leadingLabel->setGeometry(QRect(142, 2, 44, 25));
        leadingBox = new fonta::ComboBox(widget);
        leadingBox->setObjectName(QStringLiteral("leadingBox"));
        leadingBox->setGeometry(QRect(188, 5, 60, 20));
        leadingBox->setMinimumSize(QSize(60, 0));
        leadingBox->setMaximumSize(QSize(60, 16777215));
        leadingBox->setEditable(true);
        leadingBox->setMaxVisibleItems(14);
        leadingBox->setInsertPolicy(QComboBox::NoInsert);
        trackingLabel = new QLabel(widget);
        trackingLabel->setObjectName(QStringLiteral("trackingLabel"));
        trackingLabel->setGeometry(QRect(142, 27, 44, 25));
        trackingBox = new fonta::ComboBox(widget);
        trackingBox->setObjectName(QStringLiteral("trackingBox"));
        trackingBox->setGeometry(QRect(188, 30, 60, 20));
        trackingBox->setMinimumSize(QSize(60, 0));
        trackingBox->setMaximumSize(QSize(60, 16777215));
        trackingBox->setEditable(true);
        trackingBox->setMaxVisibleItems(27);
        trackingBox->setInsertPolicy(QComboBox::NoInsert);

        horizontalLayout->addWidget(widget);

        line = new QFrame(bottomWidget);
        line->setObjectName(QStringLiteral("line"));
        line->setMinimumSize(QSize(0, 0));
        line->setMaximumSize(QSize(16777215, 40));
        line->setFrameShadow(QFrame::Plain);
        line->setFrameShape(QFrame::VLine);

        horizontalLayout->addWidget(line);

        alignLeftButton = new QPushButton(bottomWidget);
        alignButtonGroup = new QButtonGroup(MainWindow);
        alignButtonGroup->setObjectName(QStringLiteral("alignButtonGroup"));
        alignButtonGroup->addButton(alignLeftButton);
        alignLeftButton->setObjectName(QStringLiteral("alignLeftButton"));
        alignLeftButton->setMinimumSize(QSize(30, 30));
        alignLeftButton->setMaximumSize(QSize(30, 30));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/pic/left.png"), QSize(), QIcon::Normal, QIcon::Off);
        alignLeftButton->setIcon(icon3);
        alignLeftButton->setCheckable(true);

        horizontalLayout->addWidget(alignLeftButton);

        alignCenterButton = new QPushButton(bottomWidget);
        alignButtonGroup->addButton(alignCenterButton);
        alignCenterButton->setObjectName(QStringLiteral("alignCenterButton"));
        alignCenterButton->setMinimumSize(QSize(30, 30));
        alignCenterButton->setMaximumSize(QSize(30, 30));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/pic/center.png"), QSize(), QIcon::Normal, QIcon::Off);
        alignCenterButton->setIcon(icon4);
        alignCenterButton->setCheckable(true);

        horizontalLayout->addWidget(alignCenterButton);

        alignRightButton = new QPushButton(bottomWidget);
        alignButtonGroup->addButton(alignRightButton);
        alignRightButton->setObjectName(QStringLiteral("alignRightButton"));
        alignRightButton->setMinimumSize(QSize(30, 30));
        alignRightButton->setMaximumSize(QSize(30, 30));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/pic/right.png"), QSize(), QIcon::Normal, QIcon::Off);
        alignRightButton->setIcon(icon5);
        alignRightButton->setCheckable(true);

        horizontalLayout->addWidget(alignRightButton);

        alignJustifyButton = new QPushButton(bottomWidget);
        alignButtonGroup->addButton(alignJustifyButton);
        alignJustifyButton->setObjectName(QStringLiteral("alignJustifyButton"));
        alignJustifyButton->setMinimumSize(QSize(30, 30));
        alignJustifyButton->setMaximumSize(QSize(30, 30));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/pic/justify.png"), QSize(), QIcon::Normal, QIcon::Off);
        alignJustifyButton->setIcon(icon6);
        alignJustifyButton->setCheckable(true);

        horizontalLayout->addWidget(alignJustifyButton);

        line_2 = new QFrame(bottomWidget);
        line_2->setObjectName(QStringLiteral("line_2"));
        line_2->setMinimumSize(QSize(0, 0));
        line_2->setMaximumSize(QSize(16777215, 40));
        line_2->setFrameShadow(QFrame::Plain);
        line_2->setFrameShape(QFrame::VLine);

        horizontalLayout->addWidget(line_2);

        textColorButton = new QPushButton(bottomWidget);
        textColorButton->setObjectName(QStringLiteral("textColorButton"));
        textColorButton->setMinimumSize(QSize(30, 30));
        textColorButton->setMaximumSize(QSize(30, 30));
        QPalette palette;
        QBrush brush(QColor(255, 0, 4, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(0, 0, 0, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Text, brush1);
        QBrush brush2(QColor(120, 120, 120, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        textColorButton->setPalette(palette);
        QFont font;
        font.setFamily(QStringLiteral("Times New Roman"));
        font.setPointSize(15);
        font.setBold(true);
        font.setItalic(false);
        font.setWeight(75);
        textColorButton->setFont(font);

        horizontalLayout->addWidget(textColorButton);

        backColorButton = new QPushButton(bottomWidget);
        backColorButton->setObjectName(QStringLiteral("backColorButton"));
        backColorButton->setMinimumSize(QSize(30, 30));
        backColorButton->setMaximumSize(QSize(30, 30));
        QPalette palette1;
        QBrush brush3(QColor(150, 150, 150, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Button, brush3);
        QBrush brush4(QColor(170, 255, 255, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Midlight, brush4);
        QBrush brush5(QColor(255, 255, 255, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Text, brush5);
        palette1.setBrush(QPalette::Active, QPalette::ButtonText, brush5);
        palette1.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette1.setBrush(QPalette::Active, QPalette::Window, brush4);
        palette1.setBrush(QPalette::Inactive, QPalette::Button, brush3);
        palette1.setBrush(QPalette::Inactive, QPalette::Midlight, brush4);
        palette1.setBrush(QPalette::Inactive, QPalette::Text, brush5);
        palette1.setBrush(QPalette::Inactive, QPalette::ButtonText, brush5);
        palette1.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::Window, brush4);
        palette1.setBrush(QPalette::Disabled, QPalette::Button, brush3);
        palette1.setBrush(QPalette::Disabled, QPalette::Midlight, brush4);
        palette1.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::Base, brush4);
        palette1.setBrush(QPalette::Disabled, QPalette::Window, brush4);
        backColorButton->setPalette(palette1);
        QFont font1;
        font1.setFamily(QStringLiteral("Times New Roman"));
        font1.setPointSize(15);
        font1.setBold(true);
        font1.setWeight(75);
        backColorButton->setFont(font1);

        horizontalLayout->addWidget(backColorButton);

        horizontalSpacer = new QSpacerItem(240, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        addFieldButton = new QPushButton(bottomWidget);
        addFieldButton->setObjectName(QStringLiteral("addFieldButton"));
        addFieldButton->setMinimumSize(QSize(30, 30));
        addFieldButton->setMaximumSize(QSize(30, 30));
        QFont font2;
        font2.setPointSize(11);
        addFieldButton->setFont(font2);

        horizontalLayout->addWidget(addFieldButton);

        removeFieldButton = new QPushButton(bottomWidget);
        removeFieldButton->setObjectName(QStringLiteral("removeFieldButton"));
        removeFieldButton->setMinimumSize(QSize(30, 30));
        removeFieldButton->setMaximumSize(QSize(30, 30));
        removeFieldButton->setFont(font2);

        horizontalLayout->addWidget(removeFieldButton);

        addFieldButton->raise();
        removeFieldButton->raise();
        line->raise();
        widget->raise();
        backColorButton->raise();
        textColorButton->raise();
        line_2->raise();
        alignLeftButton->raise();
        alignCenterButton->raise();
        alignRightButton->raise();
        alignJustifyButton->raise();

        workAreaLayout->addWidget(bottomWidget);

        fontsListSplitter->addWidget(workAreaLayoutWidget);

        verticalLayout->addWidget(fontsListSplitter);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 830, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        menuTabs = new QMenu(menuBar);
        menuTabs->setObjectName(QStringLiteral("menuTabs"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QStringLiteral("toolBar"));
        toolBar->setMovable(false);
        toolBar->setAllowedAreas(Qt::AllToolBarAreas);
        toolBar->setOrientation(Qt::Horizontal);
        toolBar->setIconSize(QSize(24, 24));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);
#ifndef QT_NO_SHORTCUT
        sizeLabel->setBuddy(sizeBox);
        leadingLabel->setBuddy(sizeBox);
        trackingLabel->setBuddy(sizeBox);
#endif // QT_NO_SHORTCUT

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuTabs->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionNew);
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSave);
        menuFile->addAction(actionSave_as);
        menuHelp->addAction(actionAbout);
        menuTabs->addAction(actionNew_Tab);
        menuTabs->addAction(actionClose_Tab);
        menuTabs->addAction(actionClose_other_Tabs);
        menuTabs->addSeparator();
        menuTabs->addAction(actionNext_Tab);
        menuTabs->addAction(actionPrevious_Tab);
        toolBar->addAction(actionFillNews);
        toolBar->addAction(actionFillPangram);
        toolBar->addAction(actionFillLoremIpsum);
        toolBar->addSeparator();

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(-1);
        trackingBox->setCurrentIndex(6);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Fonta", 0));
        actionSave_as->setText(QApplication::translate("MainWindow", "Save as...", 0));
        actionOpen->setText(QApplication::translate("MainWindow", "Open...", 0));
        actionOpen->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", 0));
        actionSave->setText(QApplication::translate("MainWindow", "Save", 0));
        actionSave->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", 0));
        actionNew->setText(QApplication::translate("MainWindow", "New", 0));
        actionNew->setShortcut(QApplication::translate("MainWindow", "Ctrl+N", 0));
        actionAbout->setText(QApplication::translate("MainWindow", "About...", 0));
        actionNew_Tab->setText(QApplication::translate("MainWindow", "New Tab", 0));
        actionNew_Tab->setShortcut(QApplication::translate("MainWindow", "Ctrl+T", 0));
        actionClose_Tab->setText(QApplication::translate("MainWindow", "Close Tab...", 0));
        actionClose_Tab->setShortcut(QApplication::translate("MainWindow", "Ctrl+W", 0));
        actionNext_Tab->setText(QApplication::translate("MainWindow", "Next Tab", 0));
        actionNext_Tab->setShortcut(QApplication::translate("MainWindow", "Ctrl+Tab", 0));
        actionPrevious_Tab->setText(QApplication::translate("MainWindow", "Previous Tab", 0));
        actionPrevious_Tab->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+Backtab", 0));
        actionClose_other_Tabs->setText(QApplication::translate("MainWindow", "Close Other Tabs...", 0));
        actionFonts_Cleaner->setText(QApplication::translate("MainWindow", "Fonts Cleaner...", 0));
        actionFillNews->setText(QApplication::translate("MainWindow", "Fill with News", 0));
#ifndef QT_NO_TOOLTIP
        actionFillNews->setToolTip(QApplication::translate("MainWindow", "Fill with News", 0));
#endif // QT_NO_TOOLTIP
        actionFillPangram->setText(QApplication::translate("MainWindow", "Fill with Pangram", 0));
#ifndef QT_NO_TOOLTIP
        actionFillPangram->setToolTip(QApplication::translate("MainWindow", "Fill with Pangram", 0));
#endif // QT_NO_TOOLTIP
        actionFillLoremIpsum->setText(QApplication::translate("MainWindow", "Fill with LoremIpsum", 0));
#ifndef QT_NO_TOOLTIP
        actionFillLoremIpsum->setToolTip(QApplication::translate("MainWindow", "Fill with LoremIpsum", 0));
#endif // QT_NO_TOOLTIP
        filterWizardButton->setText(QApplication::translate("MainWindow", "...", 0));
        sizeLabel->setText(QApplication::translate("MainWindow", "Size:", 0));
        sizeBox->clear();
        sizeBox->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "6 pt", 0)
         << QApplication::translate("MainWindow", "8 pt", 0)
         << QApplication::translate("MainWindow", "9 pt", 0)
         << QApplication::translate("MainWindow", "10 pt", 0)
         << QApplication::translate("MainWindow", "11 pt", 0)
         << QApplication::translate("MainWindow", "12 pt", 0)
         << QApplication::translate("MainWindow", "14 pt", 0)
         << QApplication::translate("MainWindow", "18 pt", 0)
         << QApplication::translate("MainWindow", "24 pt", 0)
         << QApplication::translate("MainWindow", "36 pt", 0)
         << QApplication::translate("MainWindow", "48 pt", 0)
         << QApplication::translate("MainWindow", "60 pt", 0)
         << QApplication::translate("MainWindow", "72 pt", 0)
        );
        sizeBox->setCurrentText(QApplication::translate("MainWindow", "6 pt", 0));
        styleLabel->setText(QApplication::translate("MainWindow", "Style:", 0));
        leadingLabel->setText(QApplication::translate("MainWindow", "Leading:", 0));
        leadingBox->clear();
        leadingBox->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Auto", 0)
         << QApplication::translate("MainWindow", "6 pt", 0)
         << QApplication::translate("MainWindow", "8 pt", 0)
         << QApplication::translate("MainWindow", "9 pt", 0)
         << QApplication::translate("MainWindow", "10 pt", 0)
         << QApplication::translate("MainWindow", "11 pt", 0)
         << QApplication::translate("MainWindow", "12 pt", 0)
         << QApplication::translate("MainWindow", "14 pt", 0)
         << QApplication::translate("MainWindow", "18 pt", 0)
         << QApplication::translate("MainWindow", "24 pt", 0)
         << QApplication::translate("MainWindow", "36 pt", 0)
         << QApplication::translate("MainWindow", "48 pt", 0)
         << QApplication::translate("MainWindow", "60 pt", 0)
         << QApplication::translate("MainWindow", "72 pt", 0)
        );
        leadingBox->setCurrentText(QApplication::translate("MainWindow", "Auto", 0));
        trackingLabel->setText(QApplication::translate("MainWindow", "Tracking:", 0));
        trackingBox->clear();
        trackingBox->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "-200", 0)
         << QApplication::translate("MainWindow", "-100", 0)
         << QApplication::translate("MainWindow", "-75", 0)
         << QApplication::translate("MainWindow", "-50", 0)
         << QApplication::translate("MainWindow", "-25", 0)
         << QApplication::translate("MainWindow", "-10", 0)
         << QApplication::translate("MainWindow", "0", 0)
         << QApplication::translate("MainWindow", "10", 0)
         << QApplication::translate("MainWindow", "25", 0)
         << QApplication::translate("MainWindow", "50", 0)
         << QApplication::translate("MainWindow", "75", 0)
         << QApplication::translate("MainWindow", "100", 0)
         << QApplication::translate("MainWindow", "150", 0)
         << QApplication::translate("MainWindow", "250", 0)
         << QApplication::translate("MainWindow", "500", 0)
        );
        trackingBox->setCurrentText(QApplication::translate("MainWindow", "0", 0));
        alignLeftButton->setText(QString());
        alignCenterButton->setText(QString());
        alignRightButton->setText(QString());
        alignJustifyButton->setText(QString());
        textColorButton->setText(QApplication::translate("MainWindow", "A", 0));
        backColorButton->setText(QApplication::translate("MainWindow", "A", 0));
        addFieldButton->setText(QApplication::translate("MainWindow", "+", 0));
        removeFieldButton->setText(QApplication::translate("MainWindow", "\342\200\223", 0));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", 0));
        menuTabs->setTitle(QApplication::translate("MainWindow", "Tabs", 0));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
