#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "widgets/combobox.h"
#include "widgets/about.h"
#include "widgets/workarea.h"
#include "widgets/renametabedit.h"

#include "sampler.h"
#include "filterwizard.h"

#include <QTextStream>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFileDialog>
#include <QDesktopWidget>
#include <QColorDialog>
#include <QSettings>
#include <QMessageBox>

#include <QDebug>

namespace fonta {

const Version MainWindow::versionNumber = Version(0, 5, 3);

MainWindow::MainWindow(CStringRef fileToOpen, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_aboutDialog(NULL)
    , m_currFile("")
{
    ui->setupUi(this);

    ui->fontsList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->fontsList, &QListWidget::customContextMenuRequested, this, &MainWindow::showFontListContextMenu);

    cauto boxEditSig = &QLineEdit::returnPressed;
    connect(ui->sizeBox->lineEdit(), boxEditSig, this, &MainWindow::on_sizeBox_edited);
    connect(ui->leadingBox->lineEdit(), boxEditSig, this, &MainWindow::on_leadingBox_edited);
    connect(ui->trackingBox->lineEdit(), boxEditSig, this, &MainWindow::on_trackingBox_edited);

    QStringList filterItems;
    for(int i = FilterMode::Start; i<FilterMode::End; ++i) {
        filterItems << FilterMode::toString(static_cast<FilterMode::type>(i));
    }
    ui->filterBox->addItems(filterItems);

    ui->fontFinderEdit->setListWidget(ui->fontsList);

    QTabWidget *tabs = ui->tabWidget;
    QTabBar *bar = tabs->tabBar();
    bar->setContextMenuPolicy(Qt::CustomContextMenu);
    tabs->setTabsClosable(false);
    bar->setStyleSheet("QTabBar::tab { height: 27px; }"
                       "QTabBar::close-button { image: url(:/pic/closeTab.png); }"
                       "QTabBar::close-button:hover { image: url(:/pic/closeTabHover.png); }"
    );
    connect(tabs, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTabPrompted);
    connect(bar, &QTabBar::tabBarDoubleClicked, this, &MainWindow::renameTab);
    connect(bar, &QTabBar::tabMoved, this, &MainWindow::onTabsMove);
    connect(bar, &QTabBar::customContextMenuRequested, this, &MainWindow::showTabsContextMenu);

    m_addTabButton = new QPushButton(bar);
    connect(m_addTabButton, &QPushButton::clicked, [&](){ addTab(InitType::Sampled); });
    connect(m_addTabButton, &QPushButton::clicked, this, &MainWindow::changeAddTabButtonGeometry);

    fillGroup = new QActionGroup(this);
    fillGroup->addAction(ui->actionFillNews);
    fillGroup->addAction(ui->actionFillPangram);
    fillGroup->addAction(ui->actionFillLoremIpsum);

    if(fileToOpen.isEmpty()) {
        addTab();
    } else {
        openFile(fileToOpen);
    }

    loadGeometry();

    // extend toolbar with language context buttons
    // (they are buttons because of their more appropriate look)
    extendToolBar();
}

MainWindow::~MainWindow()
{
    saveGeometry();
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    changeAddTabButtonGeometry();
}

void MainWindow::saveGeometry()
{
    QSettings settings("PitM", "Fonta");

    settings.beginGroup("FontaWindow");
    settings.setValue("geometry", QMainWindow::saveGeometry());
    settings.setValue("windowState", saveState());

    cauto sizes = ui->fontsListSplitter->sizes();
    settings.setValue("fontsSplitterSizes0", sizes[0]);
    settings.setValue("fontsSplitterSizes1", sizes[1]);
    settings.endGroup();
}

void MainWindow::loadGeometry()
{
    QSettings settings("PitM", "Fonta");

    settings.beginGroup("FontaWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    int size0 = settings.value("fontsSplitterSizes0", 100).toInt();
    int size1 = settings.value("fontsSplitterSizes1", 200).toInt();

    if(size0 != -1 && size1 != -1) {
        ui->fontsListSplitter->setSizes({size0, size1});
    }

    settings.endGroup();
}

void MainWindow::extendToolBar()
{
    int btnSize = 30;
    contextGroup = new QButtonGroup(this);
    autoButton = new QPushButton("Auto", this);
    engButton = new QPushButton("En", this);
    rusButton = new QPushButton("Ru", this);

    auto addButton = [&](QPushButton *button, LanguageContext context){
        button->setCheckable(true);
        button->setFlat(true);

        button->setMinimumHeight(btnSize);
        button->setMaximumHeight(btnSize);
        button->setMinimumWidth(btnSize);
        button->setMaximumWidth(btnSize);
        ui->toolBar->addWidget(button);

        contextGroup->addButton(button);

        connect(button, &QPushButton::toggled, [this, context](bool checked){
            if(checked) {
                m_currField->setLanguageContext(context);
            }
        });
    };

    addButton(autoButton, LanguageContext::Auto);
    addButton(engButton, LanguageContext::Eng);
    addButton(rusButton, LanguageContext::Rus);
}

void MainWindow::changeAddTabButtonGeometry()
{
    QRect r = ui->tabWidget->tabBar()->tabRect(m_workAreas.length()-1);
    const int padding = 2;
    const int sz = r.height()-3*padding;
    m_addTabButton->setGeometry(r.x() + r.width() + padding, padding, sz, sz);
}

void MainWindow::onTabsMove(int from, int to)
{
    WorkArea* toMove = m_workAreas.at(from);
    m_workAreas.removeAt(from);
    m_workAreas.insert(to, toMove);
}

void MainWindow::showFontListContextMenu(const QPoint &point)
{
    if (point.isNull()) {
        return;
    }

    QString text = ui->fontsList->itemAt(point)->text();

    QMenu menu(this);

    QAction remove(tr("Uninstall font"), this);
    connect(&remove, &QAction::triggered, this, [=](){ uninstallFont(text); });
    menu.addAction(&remove);

    menu.exec(ui->fontsList->mapToGlobal(point));
}

void MainWindow::showTabsContextMenu(const QPoint &point)
{
    if (point.isNull()) {
        return;
    }

    int tabIndex = ui->tabWidget->tabBar()->tabAt(point);
    ui->tabWidget->setCurrentIndex(tabIndex);

    QMenu menu(this);

    QAction remove(tr("Close Other Tabs"), this);
    int length = m_workAreas.size();
    if(length > 1) {
        connect(&remove, &QAction::triggered, this, &MainWindow::closeOtherTabs);
        connect(&remove, &QAction::triggered, this, &MainWindow::changeAddTabButtonGeometry);
        menu.addAction(&remove);
        menu.addSeparator();
    }

    QAction clone(tr("Clone Tab"), this);
    connect(&clone, &QAction::triggered, this, &MainWindow::cloneCurrTab);
    menu.addAction(&clone);

    menu.exec(ui->tabWidget->tabBar()->mapToGlobal(point));
}

void MainWindow::uninstallFont(const QString &fontName)
{
    cauto linked = fontaDB().linkedFonts(fontName);

    QString dialogMessage = linked.isEmpty() ?
                       tr("Uninstall %1 font?").arg(fontName) :
                       tr("Removal of %1 font will cause deletion of following fonts:\n\n%2\n\nSure?").arg(fontName, linked.join("\n"));

    int ret = callQuestionDialog(dialogMessage);

    if (ret == QMessageBox::Ok) {
        fontaDB().uninstall(fontName);
        on_filterBox_currentIndexChanged(ui->filterBox->currentIndex()); // force fonts list update
        callInfoDialog(tr("Font(s) uninstalled!\nReboot your PC for changes to take effect"));
        return;
    }
}

void MainWindow::addTab(InitType initType)
{
    int id = m_workAreas.length();

    QWidget* tab = new QWidget();
    QVBoxLayout* horizontalLayout = new QVBoxLayout(tab);
    horizontalLayout->setSpacing(0);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);
    m_currWorkArea = new WorkArea(id, tab, Sampler::getName());

    if(initType == InitType::Sampled) {
        m_currWorkArea->createSample();
    }

    QWidget *topMargin = new QWidget(tab);
    topMargin->setMinimumHeight(30);
    topMargin->setMaximumHeight(30);
    topMargin->setStyleSheet("background-color:white;");

    horizontalLayout->addWidget(topMargin);
    horizontalLayout->addWidget(m_currWorkArea);
    ui->tabWidget->addTab(tab, m_currWorkArea->name());

    m_workAreas.push_back(m_currWorkArea);

    m_currField = m_currWorkArea->currField();

    makeFieldsConnected();
    ui->tabWidget->setCurrentIndex(id);

    if(initType == InitType::Sampled) {
        m_currField->setFocus();
    }

    if(ui->tabWidget->count() > 1) {
        ui->tabWidget->setTabsClosable(true);
    }

    changeAddTabButtonGeometry();

    if(m_workAreas.count() > 1) {
        ui->actionClose_Tab->setEnabled(true);
        ui->actionClose_other_Tabs->setEnabled(true);
        ui->actionNext_Tab->setEnabled(true);
        ui->actionPrevious_Tab->setEnabled(true);
    }
}

void MainWindow::closeTabPrompted(int i)
{
    int ret = callQuestionDialog(tr("Delete %1 tab.\nSure?").arg(m_workAreas[i]->name()));

    if (ret == QMessageBox::Ok) {
        closeTab(i);
    }

    changeAddTabButtonGeometry();
}

void MainWindow::closeTab(int id)
{
    delete m_workAreas[id];
    m_workAreas.removeAt(id);
    ui->tabWidget->removeTab(id);

    for(int i = id; i<m_workAreas.length(); ++i) {
        m_workAreas[i]->setId(i);
    }
    m_currWorkArea = id < m_workAreas.length() ? m_workAreas[id] : m_workAreas.last();

    if(ui->tabWidget->count() == 1) {
        ui->tabWidget->setTabsClosable(false);
    }

    if(m_workAreas.count() <= 1) {
        ui->actionClose_Tab->setDisabled(true);
        ui->actionClose_other_Tabs->setDisabled(true);
        ui->actionNext_Tab->setDisabled(true);
        ui->actionPrevious_Tab->setDisabled(true);
    }
}

void MainWindow::closeOtherTabs()
{
    int ret = callQuestionDialog(tr("Delete all tabs except %1\nSure?").arg(m_currWorkArea->name()));

    if (ret != QMessageBox::Ok) {
        return;
    }

    WorkArea* onlyArea = m_currWorkArea;

    for(int i = 0; i<m_workAreas.size(); ++i) {
        if(m_workAreas[i] != onlyArea) {
            closeTab(i);
            --i;
        }
    }

    changeAddTabButtonGeometry();
}

void MainWindow::cloneCurrTab()
{
    WorkArea& protoArea = *m_currWorkArea;

    addTab(InitType::Empty);

    for(const Field* protoField : protoArea) {
        Field* field = protoField->clone();
        m_currWorkArea->addField(field);
        makeFieldConnected(field);
    }

    m_currWorkArea->setSizes(protoArea.sizes());
    m_currWorkArea->setCurrField((*m_currWorkArea)[0]);
    m_currField = m_currWorkArea->currField();
    m_currField->setFocus();
}

void MainWindow::renameTab(int id)
{
    RenameTabEdit* edit = new RenameTabEdit(ui->tabWidget, m_workAreas[id], ui->tabWidget->tabBar());
    connect(edit, &RenameTabEdit::applied, this, &MainWindow::changeAddTabButtonGeometry);
    edit->show();
}

void MainWindow::makeFieldConnected(Field* field) {
    connect(field, &Field::focussed, this, &MainWindow::on_currentFieldChanged);
    connect(field, &Field::contentBecameUserDefined, this, &MainWindow::resetFillActions);
    connect(field, &Field::contentBecameUserDefined, this, &MainWindow::disableContextGroup);
}

void MainWindow::makeFieldsConnected() {
    for(auto *field : *m_currWorkArea) {
        makeFieldConnected(field);
    }
}

void MainWindow::on_addFieldButton_clicked()
{
    Field* field = m_currWorkArea->addField();
    field->setFontFamily("Arial");

    makeFieldConnected(field);

    updateAddRemoveButtons();
}

void MainWindow::on_removeFieldButton_clicked()
{
    int count = m_currWorkArea->fieldCount();
    if(m_currField->id() == count-1) {
        (*m_currWorkArea)[count-2]->setFocus();
    }

    m_currWorkArea->popField();
    updateAddRemoveButtons();
}

void MainWindow::on_currentFieldChanged(Field* field)
{
    m_currField = field;
    CStringRef family = m_currField->fontFamily();

    // show family
    ui->fontFinderEdit->setText(family);

    QList<QListWidgetItem*> items = ui->fontsList->findItems(family, Qt::MatchExactly);
    if(items.size() > 0) {
        ui->fontsList->setCurrentItem(items[0]);
        ui->fontsList->scrollToItem(items[0], QAbstractItemView::PositionAtCenter);
    }

    // show size
    ui->sizeBox->lineEdit()->setText(QString::number(m_currField->fontSize()) + " pt");

    // show style
    ui->styleBox->setCurrentIndex(ui->styleBox->findText(m_currField->fontStyle()));

    // show leading
    float lead = m_currField->leading();
    if(lead == inf()) {
        ui->leadingBox->lineEdit()->setText("Auto");
    } else {
        ui->leadingBox->lineEdit()->setText(QString::number(lead) + " pt");
    }


    // show tracking
    ui->trackingBox->lineEdit()->setText(QString::number(m_currField->tracking()));

    // show alignment
    switch(m_currField->textAlignment()) {
        default:
        case (Qt::AlignLeft): ui->alignLeftButton->setChecked(true); break;
        case (Qt::AlignHCenter): ui->alignCenterButton->setChecked(true); break;
        case (Qt::AlignRight): ui->alignRightButton->setChecked(true); break;
        case (Qt::AlignJustify): ui->alignJustifyButton->setChecked(true); break;
    }

    // show content mode
    switch(m_currField->contentMode()) {
        case (ContentMode::News): ui->actionFillNews->setChecked(true); break;
        case (ContentMode::Pangram): ui->actionFillPangram->setChecked(true); break;
        case (ContentMode::LoremIpsum): ui->actionFillLoremIpsum->setChecked(true); break;
        default: {
            resetFillActions();
        }
    }

    // show language context
    updateContextGroup();
}

void MainWindow::enableContextGroup()
{
    contextGroup->setExclusive(false);
    for(auto *b : contextGroup->buttons()) {
        b->setEnabled(true);
    }
    contextGroup->setExclusive(true);
}

void MainWindow::disableContextGroup()
{
    contextGroup->setExclusive(false);
    for(auto *b : contextGroup->buttons()) {
        b->setChecked(false);
    }
    contextGroup->setExclusive(true);

    for(auto *b : contextGroup->buttons()) {
        b->setDisabled(true);
    }
}

void MainWindow::updateContextGroup()
{
    if(m_currField->contentMode() != ContentMode::UserDefined) {
        enableContextGroup();

        switch(m_currField->languageContext()) {
            case (LanguageContext::Auto): autoButton->setChecked(true); break;
            case (LanguageContext::Eng): engButton->setChecked(true); break;
            case (LanguageContext::Rus): rusButton->setChecked(true); break;
            default: {
                disableContextGroup();
            }
        }
    } else {
        disableContextGroup();
    }
}

void MainWindow::on_fontsList_currentTextChanged(const QString &family)
{
    ui->fontFinderEdit->setText(family);

    ui->styleBox->clear();
    ui->styleBox->addItems(fontaDB().styles(family));

    m_currField->setFontFamily(family);
    ui->styleBox->setCurrentText(m_currField->fontStyle());
}

void MainWindow::on_sizeBox_edited()
{
    on_sizeBox_activated(ui->sizeBox->lineEdit()->text());
}

void MainWindow::on_sizeBox_activated(const QString &arg1)
{
    float val = strtof(arg1.toStdString().c_str(), nullptr);
    m_currField->setFontSize(val);
}

void MainWindow::on_filterBox_currentIndexChanged(int index)
{
    if(ui->filterBox->currentText() == "Custom") {
        return;
    } else {
        int lastIndex = ui->filterBox->count()-1;
        if(ui->filterBox->itemText(lastIndex) == "Custom") {
            ui->filterBox->removeItem(lastIndex);
        }
    }

    ui->fontsList->clear();

    bool (DB::*goodFont)(CStringRef) const;
    switch(index) {
        default:
        case FilterMode::ALL:        goodFont = &DB::isAnyFont; break;
        case FilterMode::CYRILLIC:   goodFont = &DB::isCyrillic; break;
        case FilterMode::SERIF:      goodFont = &DB::isSerif; break;
        case FilterMode::SANS_SERIF: goodFont = &DB::isSansSerif; break;
        case FilterMode::MONOSPACE:  goodFont = &DB::isMonospaced; break;
        case FilterMode::SCRIPT:     goodFont = &DB::isScript; break;
        case FilterMode::DECORATIVE: goodFont = &DB::isDecorative; break;
        case FilterMode::SYMBOLIC:   goodFont = &DB::isSymbolic; break;
    }

    for (CStringRef family : fontaDB().families()) {

        if(!(fontaDB().*goodFont)(family)) {
            continue;
        }

        QListWidgetItem* item = new QListWidgetItem(family);

#ifdef FONTA_DETAILED_DEBUG
        FullFontInfo info = fontaDB().getFullFontInfo(family);
        QString detail;
        QTextStream ss(&detail);

        QString pad("      ");
        ss << "Qt:\n";
        if(info.qtInfo.cyrillic) ss << pad << "Cyrillic\n";
        if(info.qtInfo.symbolic) ss << pad << "Symbolic\n";
        if(info.qtInfo.monospaced) ss << pad << "Monospaced\n";

        if(info.TTFExists) {
            ss << "TTF:\n";
            ss << pad << "Family:     " << FamilyClass::toString(info.fontaTFF.familyClass) << "\n";
            ss << pad << "Family Sub: " << info.fontaTFF.familySubClass << "\n";
            ss << pad << "Panose: " << info.fontaTFF.panose.getNumberAsString() << "\n";
            if(info.fontaTFF.cyrillic) ss << pad << "Cyrillic\n";
            if(info.fontaTFF.monospaced) ss << pad << "Monospaced\n";
            ss << pad << "Files: " << info.fontaTFF.files.toList().join(' ') << "\n";
            if(!info.fontaTFF.linkedFonts.isEmpty()) {
                ss << pad << "Linked fonts: " << info.fontaTFF.linkedFonts.toList().join(' ');
            }
        } else {
            qWarning() << family << qPrintable("doesn't have TTF");
        }

        if(detail.endsWith('\n') ) {
            detail.truncate(detail.size()-1);
        }

        item->setToolTip(detail);
#endif

        ui->fontsList->addItem(item);
        ui->statusBar->showMessage(QString("%1 fonts").arg(ui->fontsList->count()));
    }
}

void MainWindow::on_styleBox_activated(CStringRef style)
{
    m_currField->setPreferableFontStyle(style);
}

void MainWindow::on_leadingBox_edited()
{
    on_leadingBox_activated(ui->leadingBox->lineEdit()->text());
}

void MainWindow::on_leadingBox_activated(CStringRef arg1)
{
    float val = inf();
    if(arg1 != "Auto") {
        val = strtof(arg1.toStdString().c_str(), nullptr);
    }

    m_currField->setLeading(val);
}

void MainWindow::on_trackingBox_edited()
{
    on_trackingBox_activated(ui->trackingBox->lineEdit()->text());
}

void MainWindow::on_trackingBox_activated(CStringRef arg1)
{
    int val = strtol(arg1.toStdString().c_str(), nullptr, 10);
    m_currField->setTracking(val);
}

void MainWindow::save(CStringRef fileName) const
{
    QJsonObject json;

    QJsonObject version;
    version["major"] = versionNumber.major;
    version["minor"] = versionNumber.minor;
    version["build"] = versionNumber.build;
    json["version"] = version;

    QJsonArray workAreasArr;
    for(auto workArea : m_workAreas) {
        QJsonObject json;
        workArea->save(json);
        workAreasArr.append(json);
    }

    json["workAreas"] = workAreasArr;
    json["currWorkArea"] = m_currWorkArea->id();

    QFile saveFile(fileName);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return;
    }

    QJsonDocument saveDoc(json);
    saveFile.write(saveDoc.toJson());
}

void MainWindow::on_actionSave_as_triggered()
{
    QSettings fontaReg("PitM", "Fonta");
    QString saveFilePath = fontaReg.value("OpenSaveFilePath", QDir::homePath()).toString();

    QString filename =
            QFileDialog::getSaveFileName(this, tr("Save Fonta"), saveFilePath, tr("Fonta files (*.fonta)"));

    if(!filename.isNull()) {
        save(filename);
        setCurrFile(filename);

        QFileInfo info(filename);
        fontaReg.setValue("OpenSaveFilePath", info.path());
    }
}

void MainWindow::load(CStringRef fileName)
{
    QFile loadFile(fileName);

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
        return;
    }

    clearWorkAreas();

    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    QJsonObject json = loadDoc.object();

    QJsonArray workAreas = json["workAreas"].toArray();
    for(const QJsonValue& workArea : workAreas) {
        addTab(InitType::Empty);
        QJsonObject areaJson = workArea.toObject();
        m_currWorkArea->load(areaJson);
        makeFieldsConnected();
        ui->tabWidget->setTabText(m_currWorkArea->id(), m_currWorkArea->name());
    }

    int workAreaId = json["currWorkArea"].toInt(0);
    ui->tabWidget->setCurrentIndex(workAreaId);

    m_currField = m_currWorkArea->currField();
    m_currField->setFocus();
}

void MainWindow::openFile(CStringRef filename)
{
    load(filename);
    setCurrFile(filename);

    changeAddTabButtonGeometry();

    QFileInfo info(filename);
    QSettings fontaReg("PitM", "Fonta");
    fontaReg.setValue("OpenSaveFilePath", info.filePath());
}

void MainWindow::on_actionOpen_triggered()
{
    QSettings fontaReg("PitM", "Fonta");
    QString saveFilePath = fontaReg.value("OpenSaveFilePath", QDir::homePath()).toString();

    QString filename =
            QFileDialog::getOpenFileName(this, tr("Open Fonta"), saveFilePath, tr("Fonta files (*.fonta)"));

    if(!filename.isNull()) {
        openFile(filename);
    }
}

void MainWindow::on_actionSave_triggered()
{
    if(m_currFile.isEmpty()) {
        on_actionSave_as_triggered();
    } else {
        save(m_currFile);
    }
}

void MainWindow::setCurrFile(CStringRef filename)
{
    m_currFile = filename;
    QFileInfo fileInfo(filename);
    setWindowTitle(tr("Fonta - %1").arg(fileInfo.fileName()));
}

void MainWindow::resetCurrFile()
{
    m_currFile.clear();
    setWindowTitle(tr("Fonta"));
}

void MainWindow::clearWorkAreas()
{
    int prevSize = m_workAreas.size();
    for(int i = 0; i<prevSize; ++i) {
        QWidget* w = ui->tabWidget->widget(0);
        w->deleteLater();
    }
    this->m_workAreas.clear();
    ui->tabWidget->clear();
}

void MainWindow::on_actionNew_triggered()
{
    resetCurrFile();
    clearWorkAreas();
    addTab();
}

void MainWindow::updateAddRemoveButtons()
{
    ui->addFieldButton->setEnabled(m_currWorkArea->fieldCount() < 9);
    ui->removeFieldButton->setEnabled(m_currWorkArea->fieldCount() > 1);
}

void MainWindow::setCurrWorkArea(int id)
{
    if(m_workAreas.isEmpty()) {
        return;
    }

    m_currWorkArea = m_workAreas[id];
    if(m_currWorkArea->fieldCount()) {
        m_currField = m_currWorkArea->currField();
        m_currField->setFocus();
    }
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if(index == -1) {
        return;
    }

    setCurrWorkArea(index);
    updateAddRemoveButtons();
}


void MainWindow::on_actionAbout_triggered()
{
    if(m_aboutDialog == NULL) {
        m_aboutDialog = new About(versionNumber, this);
    }
    m_aboutDialog->show();
}

void MainWindow::on_filterWizardButton_clicked()
{
    FilterWizard* w = new FilterWizard(this);
    w->exec();
}

void MainWindow::filterFontList(const QStringList& l)
{
    ui->fontsList->clear();
    ui->fontsList->addItems(l);
    ui->filterBox->addItem("Custom");
    ui->filterBox->setCurrentText("Custom");
}

void MainWindow::on_backColorButton_clicked()
{
    QColor c = QColorDialog::getColor(m_currField->sheet()["background-color"], this);

    if(c.isValid()) {
        m_currField->sheet().set("background-color", c.red(), c.green(), c.blue());
        m_currField->applySheet();
    }
}

void MainWindow::on_textColorButton_clicked()
{
    QColor c = QColorDialog::getColor(m_currField->sheet()["color"], this);

    if(c.isValid()) {
        m_currField->sheet().set("color", c.red(), c.green(), c.blue());
        m_currField->applySheet();
    }
}

void MainWindow::on_actionNew_Tab_triggered()
{
    addTab();
}

void MainWindow::on_actionClose_Tab_triggered()
{
    if(m_workAreas.count() > 1) {
        closeTabPrompted(m_currWorkArea->id());
    }
}

void MainWindow::on_actionClose_other_Tabs_triggered()
{
    if(m_workAreas.count() > 1) {
        closeOtherTabs();
    }
}

void MainWindow::on_actionNext_Tab_triggered()
{
    int id = m_currWorkArea->id();
    if(id != m_workAreas.count()-1) {
        ++id;
    } else {
        id = 0;
    }

    m_currWorkArea = m_workAreas[id];
    ui->tabWidget->setCurrentIndex(id);
}

void MainWindow::on_alignLeftButton_toggled()
{
    m_currField->alignText(Qt::AlignLeft);
}

void MainWindow::on_alignCenterButton_toggled()
{
    m_currField->alignText(Qt::AlignHCenter);
}

void MainWindow::on_alignRightButton_toggled()
{
    m_currField->alignText(Qt::AlignRight);
}

void MainWindow::on_alignJustifyButton_toggled()
{
    m_currField->alignText(Qt::AlignJustify);
}

void MainWindow::on_actionFillNews_triggered()
{
    m_currField->setContentMode(ContentMode::News);
    updateContextGroup();
}

void MainWindow::on_actionFillPangram_triggered()
{
    m_currField->setContentMode(ContentMode::Pangram);
    updateContextGroup();
}

void MainWindow::on_actionFillLoremIpsum_triggered()
{
    m_currField->setContentMode(ContentMode::LoremIpsum);
    updateContextGroup();
}

void MainWindow::resetFillActions()
{
    ui->actionFillNews->setChecked(false);
    ui->actionFillPangram->setChecked(false);
    ui->actionFillLoremIpsum->setChecked(false);
}

} // namespace fonta
