#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "widgets/combobox.h"
#include "widgets/about.h"
#include "widgets/workarea.h"
#include "widgets/renametabedit.h"

#include "utils.h"
#include "sampler.h"
#include "filterwizard.h"

#include <QTextStream>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFileDialog>
#include <QDesktopWidget>
#include <QColorDialog>
#include <QMessageBox>
#include <QTranslator>
#include <QLibraryInfo>

#include <QDebug>

namespace fonta {

const QVersionNumber MainWindow::versionNumber = QVersionNumber(1, 0, 0);

static void updateFilterBox(QComboBox *filterBox)
{
    filterBox->clear();
    QStringList filterItems;
    for(int i = FilterMode::Start; i<FilterMode::End; ++i) {
        filterItems << FilterMode::toString(static_cast<FilterMode::type>(i));
    }
    filterBox->addItems(filterItems);
}

MainWindow::MainWindow(CStringRef fileToOpen, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_settings("PitM", "Fonta")
{
    ui->setupUi(this);

    ui->fontsList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->fontsList, &QListWidget::customContextMenuRequested, this, &MainWindow::showFontListContextMenu);

    cauto boxEditSig = &QLineEdit::returnPressed;
    connect(ui->sizeBox->lineEdit(), boxEditSig, this, &MainWindow::onSizeBoxEdited);
    connect(ui->leadingBox->lineEdit(), boxEditSig, this, &MainWindow::onLeadingBoxEdited);
    connect(ui->trackingBox->lineEdit(), boxEditSig, this, &MainWindow::onTrackingBoxEdited);

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

    // extend toolbar with language context buttons
    // (they are buttons because of their more appropriate look)
    extendToolBar();
    setToolTips();

    loadGeometry();

    QVariant langVariant = m_settings.value("Language");
    QString lang = langVariant.isNull() ? QLocale::system().name().left(2) : langVariant.toString();
    setLanguage(lang);

    if(fileToOpen.isEmpty()) {
        addTab();
    } else {
        openFile(fileToOpen);
    }

    updateFilterBox(ui->filterBox);
    connect(ui->filterBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::currentFilterBoxIndexChanged);
    currentFilterBoxIndexChanged(0);
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

template <class T>
static void setToolTip(T *w, const QString &brief, const QString &details)
{
    w->setToolTip(QString("<b>") + brief + "</b><p>" + details + "</p>");
}

void MainWindow::setToolTips()
{
    qApp->setStyleSheet("QToolTip {padding: 6px; background-color: #FFFFFF}");

    fonta::setToolTip(ui->alignLeftButton, tr("Align left"), tr("Aligns text to the left side of textbox."));
    fonta::setToolTip(ui->alignRightButton, tr("Align right"), tr("Aligns text to the right side of textbox."));
    fonta::setToolTip(ui->alignCenterButton, tr("Center"), tr("Aligns text horizontally to the center of textbox."));
    fonta::setToolTip(ui->alignJustifyButton, tr("Justify"), tr("Justifies all your text like in newspapers."));

    fonta::setToolTip(ui->textColorButton, tr("Text color"), tr("Allows you to set custom text color."));
    fonta::setToolTip(ui->backColorButton, tr("Background color"), tr("Allows you to set custom background color of textbox."));

    fonta::setToolTip(ui->addFieldButton, tr("Add textbox"), tr("Adds new textbox to the bottom of the working tab."));
    fonta::setToolTip(ui->removeFieldButton, tr("Remove textbox"), tr("Removes last textbox from working tab."));

    fonta::setToolTip(ui->filterWizardButton, tr("Advanced fonts filtering"), tr("Shows filtering wizard that helps you to flexibly customize the list of fonts."));
    fonta::setToolTip(ui->filterBox, tr("Fonts filter"), tr("Quick fonts filters."));

    fonta::setToolTip(ui->actionFillNews, tr("Fill with news"), tr("Fill textbox with random RSS news."));
    fonta::setToolTip(ui->actionFillPangram, tr("Fill with pangram"), tr("Fill textbox with <i>Quick brown fox</i> pangram."));
    fonta::setToolTip(ui->actionFillLoremIpsum, tr("Fill with Lorem Ipsum"), tr("Fill textbox with special dummy text."));

    fonta::setToolTip(autoButton, tr("Auto language"), tr("Select language to fill a textbox depending on fonts language support. If it supports cyrillic then it is preffered."));
    fonta::setToolTip(engButton, tr("Force Eng language"), tr("Always fill textbox with latin text."));
    fonta::setToolTip(rusButton, tr("Force Rus language"), tr("Always fill textbox with cyrillic text."));

    fonta::setToolTip(ui->sizeLabel, tr("Font size"), tr("Set font size in pts."));
    fonta::setToolTip(ui->styleLabel, tr("Font style"), tr("Select font style. Ex.: <i>Regular, Italic, Bold, Bold Italic</i>."));
    fonta::setToolTip(ui->leadingLabel, tr("Textbox leading"), tr("Set textbox line spacing in pts. <i>Auto</i> means 120% from font size."));
    fonta::setToolTip(ui->trackingLabel, tr("Textbox tracking"), tr("Set letter spacing in 1/1000 ems."));
}

void MainWindow::saveGeometry()
{
    m_settings.beginGroup("FontaWindow");
    m_settings.setValue("geometry", QMainWindow::saveGeometry());
    m_settings.setValue("windowState", saveState());

    cauto sizes = ui->fontsListSplitter->sizes();
    m_settings.setValue("fontsSplitterSizes0", sizes[0]);
    m_settings.setValue("fontsSplitterSizes1", sizes[1]);
    m_settings.endGroup();
}

void MainWindow::loadGeometry()
{
    m_settings.beginGroup("FontaWindow");
    restoreGeometry(m_settings.value("geometry").toByteArray());
    restoreState(m_settings.value("windowState").toByteArray());

    int size0 = m_settings.value("fontsSplitterSizes0", 100).toInt();
    int size1 = m_settings.value("fontsSplitterSizes1", 200).toInt();

    if(size0 != -1 && size1 != -1) {
        ui->fontsListSplitter->setSizes({size0, size1});
    }

    m_settings.endGroup();
}

void MainWindow::extendToolBar()
{
    int btnSize = 30;
    contextGroup = new QButtonGroup(this);
    autoButton = new QPushButton(tr("Auto"), this);
    engButton = new QPushButton(tr("En"), this);
    rusButton = new QPushButton(tr("Ru"), this);

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
        currentFilterBoxIndexChanged(ui->filterBox->currentIndex()); // force fonts list update
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

    m_currWorkArea = new WorkArea(id, tab, Sampler::instance()->getName());

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
    connect(field, &Field::swapRequested, this, &MainWindow::swapFonts);
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

void MainWindow::on_currentFieldChanged()
{
    m_currField = qobject_cast<Field *>(sender());

    if(m_swapRequester) {
        m_currField->swapFamiliesWith(m_swapRequester);
        swapBlockState(false);
        m_swapRequester = nullptr;
    }

    updateFontFamily();

    // show size
    ui->sizeBox->lineEdit()->setText(tr("%1 pt").arg(QString::number(m_currField->fontSize())));

    // show style
    ui->styleBox->setCurrentIndex(ui->styleBox->findText(m_currField->fontStyle()));

    // show leading
    float lead = m_currField->leading();
    if(lead == inf()) {
        ui->leadingBox->lineEdit()->setText(tr("Auto"));
    } else {
        ui->leadingBox->lineEdit()->setText(tr("%1 pt").arg(QString::number(lead)));
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

void MainWindow::updateFontFamily()
{
    CStringRef family = m_currField->fontFamily();

    // show family
    ui->fontFinderEdit->setText(family);

    QList<QListWidgetItem*> items = ui->fontsList->findItems(family, Qt::MatchExactly);
    if(items.size() > 0) {
        ui->fontsList->setCurrentItem(items[0]);
        ui->fontsList->scrollToItem(items[0], QAbstractItemView::PositionAtCenter);
    }
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

void MainWindow::onSizeBoxEdited()
{
    on_sizeBox_activated(ui->sizeBox->lineEdit()->text());
}

void MainWindow::on_sizeBox_activated(const QString &arg1)
{
    float val = strtof(arg1.toStdString().c_str(), nullptr);
    m_currField->setFontSize(val);
}

void MainWindow::currentFilterBoxIndexChanged(int index)
{
    if(ui->filterBox->currentText() == tr("Custom")) {
        return;
    } else {
        int lastIndex = ui->filterBox->count()-1;
        if(ui->filterBox->itemText(lastIndex) == tr("Custom")) {
            ui->filterBox->removeItem(lastIndex);
        }
    }

    // preserve family
    QString currFamily;
    if(m_currField) {
         currFamily = m_currField->fontFamily();
    }

    ui->fontsList->clear();

    bool (DB::*goodFont)(CStringRef) const;
    switch(index) {
        default:
        case FilterMode::All:        goodFont = &DB::isAnyFont; break;
        case FilterMode::Cyrillic:   goodFont = &DB::isCyrillic; break;
        case FilterMode::Serif:      goodFont = &DB::isSerif; break;
        case FilterMode::SansSerif:  goodFont = &DB::isSansSerif; break;
        case FilterMode::Monospace:  goodFont = &DB::isMonospaced; break;
        case FilterMode::Script:     goodFont = &DB::isScript; break;
        case FilterMode::Decorative: goodFont = &DB::isDecorative; break;
        case FilterMode::Symbolic:   goodFont = &DB::isSymbolic; break;
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
            ss << pad << "Family:     " << FamilyClass::toString(info.fontaTFF->familyClass) << "\n";
            ss << pad << "Family Sub: " << info.fontaTFF->familySubClass << "\n";
            ss << pad << "Panose: " << info.fontaTFF->panose.getNumberAsString() << "\n";
            if(info.fontaTFF->cyrillic) ss << pad << "Cyrillic\n";
            ss << pad << "Files: " << info.fontaTFF->files.toList().join(' ') << "\n";
            if(!info.fontaTFF->linkedFonts.isEmpty()) {
                ss << pad << "Linked fonts: " << info.fontaTFF->linkedFonts.toList().join(' ');
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
    }

    ui->statusBar->showMessage(tr("%1 fonts").arg(ui->fontsList->count()));

    if(m_currField) {
        m_currField->setFontFamily(currFamily);
    }
}

void MainWindow::on_styleBox_activated(CStringRef style)
{
    m_currField->setPreferableFontStyle(style);
}

void MainWindow::onLeadingBoxEdited()
{
    on_leadingBox_activated(ui->leadingBox->lineEdit()->text());
}

void MainWindow::on_leadingBox_activated(CStringRef arg1)
{
    float val = inf();
    if(arg1 != tr("Auto")) {
        val = strtof(arg1.toStdString().c_str(), nullptr);
    }

    m_currField->setLeading(val);
}

void MainWindow::onTrackingBoxEdited()
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
    version["major"] = versionNumber.majorVersion();
    version["minor"] = versionNumber.minorVersion();
    version["build"] = versionNumber.microVersion();
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
    QString saveFilePath = m_settings.value("OpenSaveFilePath", QDir::homePath()).toString();

    QString filename =
            QFileDialog::getSaveFileName(this, tr("Save Fonta"), saveFilePath, tr("Fonta files (*.fonta)"));

    if(!filename.isNull()) {
        save(filename);
        setCurrFile(filename);

        QFileInfo info(filename);
        m_settings.setValue("OpenSaveFilePath", info.path());
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
    m_settings.setValue("OpenSaveFilePath", info.filePath());
}

void MainWindow::on_actionOpen_triggered()
{
    QString saveFilePath = m_settings.value("OpenSaveFilePath", QDir::homePath()).toString();

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

void MainWindow::filterFontList(const QStringList& l, FilterMode::type mode)
{
    if(mode != FilterMode::Custom) {
        ui->filterBox->setCurrentText(FilterMode::toString(mode));
        return;
    }

    // preserve family
    QString currFamily = m_currField->fontFamily();

    ui->fontsList->clear();
    ui->fontsList->addItems(l);

    const QString customString = tr("Custom");

    if(ui->filterBox->itemText(ui->filterBox->count()-1) != customString) {
        ui->filterBox->addItem(customString);
    }

    ui->filterBox->setCurrentText(customString);
    ui->statusBar->showMessage(tr("%1 fonts").arg(l.count()));

    m_currField->setFontFamily(currFamily);
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

static void setEnabled(QLayout *layout, bool enabled)
{
    for(int i = 0; i<layout->count(); ++i) {
        QLayoutItem *item = layout->itemAt(i);

        QWidget *w = item->widget();
        if(w) {
            w->setEnabled(enabled);
            continue;
        }

        QLayout *l = item->layout();
        setEnabled(l, enabled);
    }
}

static void setDisabled(QLayout *layout, bool enabled)
{
    setEnabled(layout, !enabled);
}

void MainWindow::swapBlockState(bool enable)
{
    ::fonta::setDisabled(ui->fontsListLayout, enable);
    ::fonta::setDisabled(ui->controlLayout, enable);
    ui->toolBar->setDisabled(enable);
    ui->menuBar->setDisabled(enable);
    ui->tabWidget->tabBar()->setDisabled(enable);
}

void MainWindow::swapFonts()
{
    int fieldsCount = m_currWorkArea->count();
    if(fieldsCount == 1) {
        return;
    }

    if(fieldsCount == 2) {
        Field *f1 = (*m_currWorkArea)[0];
        Field *f2 = (*m_currWorkArea)[1];

        f1->swapFamiliesWith(f2);
        updateFontFamily();
    } else {
        swapBlockState(true);
        m_swapRequester = qobject_cast<Field *>(sender());
    }
}

void MainWindow::setLanguage(const QString &lang)
{
    if(lang == m_currLanguage) {
        return;
    }

    if(m_translator) {
        qApp->removeTranslator(m_translator);
        delete m_translator;
    }
    m_translator = new QTranslator();
    if (m_translator->load(lang, ":/i18n")) {
        qApp->installTranslator(m_translator);
    }

    if(m_qtTranslator) {
        qApp->removeTranslator(m_qtTranslator);
        delete m_qtTranslator;
    }
    m_qtTranslator = new QTranslator();
    if (m_qtTranslator->load("qtbase_" + lang, QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
        qApp->installTranslator(m_qtTranslator);
    }

    m_currLanguage = lang;
    m_settings.setValue("Language", lang);

    ui->retranslateUi(this);
    setToolTips();
    updateFilterBox(ui->filterBox);
}

void MainWindow::on_actionEnglish_triggered()
{
    setLanguage("en");
}

void MainWindow::on_actionRussian_triggered()
{
    setLanguage("ru");
}

} // namespace fonta
