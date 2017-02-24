#include "filterwizard.h"

#include "fontadb.h"
#include "mainwindow.h"

#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QVariant>
#include <QRect>

namespace fonta {

FilterWizard::FilterWizard(QWidget *parent)
    : QWizard(parent)
{
    setPage(Page_General, new GeneralPage);
    setPage(Page_SerifFamily, new SerifFamilyPage);
    setPage(Page_SansFamily, new SansFamilyPage);
    setPage(Page_Finish, new FinishPage);

    setStartId(Page_General);
    setWizardStyle(ModernStyle);

    setWindowTitle(tr("Filter Wizard"));

    QRect parentG = dynamic_cast<MainWindow*>(parent)->geometry();
    int wid = 423;
    int hei = 400;//364;
    QRect g(parentG.x() + (parentG.width()-wid)/2, parentG.y() + 50, wid, hei);
    setMinimumSize(wid, hei);
    setMaximumSize(wid, hei);

    setGeometry(g);
}

void FilterWizard::accept()
{
#define declBool(X) bool X = field(#X).toBool()

    declBool(serif);
    declBool(sans);
    declBool(script);
    declBool(display);
    declBool(symbolic);

    declBool(oldstyle);
    declBool(transitional);
    declBool(modern);
    declBool(slab);

    declBool(grotesque);
    declBool(geometric);
    declBool(humanist);

    declBool(monospaced);
    declBool(cyrillic);

#undef declBool

    if(!serif && !sans && !script && !display && !symbolic) {
        serif = sans = script = display = symbolic = true;
    }

    if(!oldstyle && !transitional && !modern && !slab) {
        oldstyle = transitional = modern = slab = true;
    }

    if(!grotesque && !geometric && !humanist) {
        grotesque = geometric = humanist = true;
    }

    DB& db = fontaDB();

    QStringList l;
    for (CStringRef f : db.families()) {
        if(monospaced && !db.isMonospaced(f)) { continue; }
        if(cyrillic && !db.isCyrillic(f)) { continue; }

        if(serif) {
            bool ok = false;
            ok |= oldstyle && db.isOldStyle(f);
            ok |= transitional && db.isTransitional(f);
            ok |= modern && db.isModern(f);
            ok |= slab && db.isSlab(f);
            if(!ok) { goto sans_lbl; }

            l << f;
            continue;
        }

    sans_lbl:
        if(sans) {
            bool ok = false;
            ok |= grotesque && db.isGrotesque(f);
            ok |= geometric && db.isGeometric(f);
            ok |= humanist && db.isHumanist(f);
            if(!ok) { goto script_lbl; }

            l << f;
            continue;
        }

    script_lbl:
        if(script) {
            if(db.isScript(f)) {
                l << f;
                continue;
            }
        }

        if(display) {
            if(db.isDecorative(f)) {
                l << f;
                continue;
            }
        }

        if(symbolic) {
            if(db.isSymbolic(f)) {
                l << f;
                continue;
            }
        }
    }

    dynamic_cast<MainWindow*>(parent())->filterFontList(l);
    QDialog::accept();
}

void GeneralPage::addGeneralBloc(QPushButton** button, int width, int height, CStringRef blockName)
{
    *button = new QPushButton();
    (*button)->setCheckable(true);
    (*button)->setStyleSheet(
        "QPushButton{"
          "background-image: url(:/pic/filter/filter_buttons/categories/" + blockName + "_inactive.png);"
          "width: " + QString::number(width) + ";"
          "height: " + QString::number(height) + ";"
          "max-width: " + QString::number(width) + ";"
          "max-height: " + QString::number(height) + ";"
          "border: none;"
        "}"
        "QPushButton::hover  {background-image: url(:/pic/filter/filter_buttons/categories/" + blockName + "_hover.png);}"
        "QPushButton::checked{background-image: url(:/pic/filter/filter_buttons/categories/" + blockName + "_active.png);}"
    );

    registerField(blockName, *button);
}

GeneralPage::GeneralPage(QWidget *parent)
    : QWizardPage(parent)
{
    addGeneralBloc(&serifButton, 210, 144, "serif");
    addGeneralBloc(&sansButton, 209, 144, "sans");
    addGeneralBloc(&scriptButton, 170, 171, "script");
    addGeneralBloc(&displayButton, 166, 171, "display");
    addGeneralBloc(&symbolicButton, 83, 171, "symbolic");

    QHBoxLayout* hor1Layout = new QHBoxLayout;
    hor1Layout->setSpacing(0);
    hor1Layout->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout* hor2Layout = new QHBoxLayout;
    hor2Layout->setSpacing(0);
    hor2Layout->setContentsMargins(0, 0, 0, 0);

    hor1Layout->addWidget(serifButton);
    hor1Layout->addWidget(sansButton);
    hor2Layout->addWidget(scriptButton);
    hor2Layout->addWidget(displayButton);
    hor2Layout->addWidget(symbolicButton);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(hor1Layout);
    layout->addLayout(hor2Layout);

    QLabel* descr = new QLabel(tr("\nChoose font categories. For advanced filtering toogle the checkbox."));
    descr->setStyleSheet("qproperty-alignment: AlignCenter;");
    layout->addWidget(descr);

    layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(layout);
}

int GeneralPage::nextId() const
{
    bool serif = field("serif").toBool();

    bool sans = field("sans").toBool();

    if(serif) {
        return FilterWizard::Page_SerifFamily;
    } else if(sans) {
        return FilterWizard::Page_SansFamily;
    }

    return FilterWizard::Page_Finish;
}

void SerifFamilyPage::addGeneralBloc(QPushButton** button, int width, int height, CStringRef blockName)
{
    *button = new QPushButton();
    (*button)->setCheckable(true);
    (*button)->setStyleSheet(
        "QPushButton{"
          "background-image: url(:/pic/filter/filter_buttons/serif_families/" + blockName + "_inactive.png);"
          "width: " + QString::number(width) + ";"
          "height: " + QString::number(height) + ";"
          "max-width: " + QString::number(width) + ";"
          "max-height: " + QString::number(height) + ";"
          "border: none;"
        "}"
        "QPushButton::hover  {background-image: url(:/pic/filter/filter_buttons/serif_families/" + blockName + "_hover.png);}"
        "QPushButton::checked{background-image: url(:/pic/filter/filter_buttons/serif_families/" + blockName + "_active.png);}"
    );

    registerField(blockName, *button);
}

SerifFamilyPage::SerifFamilyPage(QWidget *parent)
    : QWizardPage(parent)
{
    addGeneralBloc(&oldstyle, 210, 156, "oldstyle");
    addGeneralBloc(&transitional, 209, 156, "transitional");
    addGeneralBloc(&modern, 210, 159, "modern");
    addGeneralBloc(&slab, 209, 159, "slab");

    QHBoxLayout* hor1Layout = new QHBoxLayout;
    hor1Layout->setSpacing(0);
    hor1Layout->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout* hor2Layout = new QHBoxLayout;
    hor2Layout->setSpacing(0);
    hor2Layout->setContentsMargins(0, 0, 0, 0);

    hor1Layout->addWidget(oldstyle);
    hor1Layout->addWidget(transitional);
    hor2Layout->addWidget(modern);
    hor2Layout->addWidget(slab);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(hor1Layout);
    layout->addLayout(hor2Layout);

    QLabel* descr = new QLabel(tr("\nChoose serif family."));
    descr->setStyleSheet("qproperty-alignment: AlignCenter;");
    layout->addWidget(descr);

    layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(layout);
}

int SerifFamilyPage::nextId() const
{
    return FilterWizard::Page_Finish;
}

void SansFamilyPage::addGeneralBloc(QPushButton** button, int width, int height, CStringRef blockName)
{
    *button = new QPushButton();
    (*button)->setCheckable(true);
    (*button)->setStyleSheet(
        "QPushButton{"
          "background-image: url(:/pic/filter/filter_buttons/sans_families/" + blockName + "_inactive.png);"
          "width: " + QString::number(width) + ";"
          "height: " + QString::number(height) + ";"
          "max-width: " + QString::number(width) + ";"
          "max-height: " + QString::number(height) + ";"
          "border: none;"
        "}"
        "QPushButton::hover  {background-image: url(:/pic/filter/filter_buttons/sans_families/" + blockName + "_hover.png);}"
        "QPushButton::checked{background-image: url(:/pic/filter/filter_buttons/sans_families/" + blockName + "_active.png);}"
    );

    registerField(blockName, *button);
}

SansFamilyPage::SansFamilyPage(QWidget *parent)
    : QWizardPage(parent)
{
    addGeneralBloc(&grotesque, 419, 150, "grotesque");
    addGeneralBloc(&geometric, 210, 165, "geometric");
    addGeneralBloc(&humanist, 209, 165, "humanist");

    QHBoxLayout* hor1Layout = new QHBoxLayout;
    hor1Layout->setSpacing(0);
    hor1Layout->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout* hor2Layout = new QHBoxLayout;
    hor2Layout->setSpacing(0);
    hor2Layout->setContentsMargins(0, 0, 0, 0);

    hor1Layout->addWidget(grotesque);
    hor2Layout->addWidget(geometric);
    hor2Layout->addWidget(humanist);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(hor1Layout);
    layout->addLayout(hor2Layout);

    QLabel* descr = new QLabel(tr("\nChoose sans family."));
    descr->setStyleSheet("qproperty-alignment: AlignCenter;");
    layout->addWidget(descr);

    layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(layout);
}

int SansFamilyPage::nextId() const
{
    return FilterWizard::Page_Finish;
}

FinishPage::FinishPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Last step..."));
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/pic/filter/intro.jpg"));

    QLabel* topLabel = new QLabel(tr("Specify additional options if you wish.\nHope you'll be satisfied with results!"));

    cyrillicBox = new QCheckBox(tr("Cyrillic support"));
    monospacedBox = new QCheckBox(tr("Monospaced"));

    registerField("cyrillic", cyrillicBox);
    registerField("monospaced", monospacedBox);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addStretch(1);
    layout->addWidget(cyrillicBox);
    layout->addWidget(monospacedBox);
    layout->addStretch(3);
    setLayout(layout);
}

int FinishPage::nextId() const
{
    return -1;
}

} // namespace fonta
