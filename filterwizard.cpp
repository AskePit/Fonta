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

GeneralPage::GeneralPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Choose font categories"));
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/pic/filter/intro.jpg"));

    serifButton = new QCheckBox(tr("Serif"));
    sansButton = new QCheckBox(tr("Sans"));
    scriptButton = new QCheckBox(tr("Script"));
    displayButton = new QCheckBox(tr("Display"));
    symbolicButton = new QCheckBox(tr("Symbolic"));

    registerField("serif", serifButton);
    registerField("sans", sansButton);
    registerField("script", scriptButton);
    registerField("display", displayButton);
    registerField("symbolic", symbolicButton);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addStretch(1);
    layout->addWidget(serifButton);
    layout->addWidget(sansButton);
    layout->addWidget(scriptButton);
    layout->addWidget(displayButton);
    layout->addWidget(symbolicButton);
    layout->addStretch(3);
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

SerifFamilyPage::SerifFamilyPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Choose serif families"));
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/pic/filter/intro.jpg"));

    oldstyle = new QCheckBox(tr("Oldstyle Serif"));
    transitional = new QCheckBox(tr("Transitional Serif"));
    modern = new QCheckBox(tr("Modern Serif"));
    slab = new QCheckBox(tr("Slab Serif"));

    registerField("oldstyle", oldstyle);
    registerField("transitional", transitional);
    registerField("modern", modern);
    registerField("slab", slab);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addStretch(1);
    layout->addWidget(oldstyle);
    layout->addWidget(transitional);
    layout->addWidget(modern);
    layout->addWidget(slab);
    layout->addStretch(3);
    setLayout(layout);
}

int SerifFamilyPage::nextId() const
{
    return FilterWizard::Page_Finish;
}

SansFamilyPage::SansFamilyPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Choose sans families"));
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/pic/filter/intro.jpg"));

    grotesque = new QCheckBox(tr("Grotesque Sans"));
    geometric = new QCheckBox(tr("Geometric Sans"));
    humanist = new QCheckBox(tr("Humanist Sans"));

    registerField("grotesque", grotesque);
    registerField("geometric", geometric);
    registerField("humanist", humanist);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addStretch(1);
    layout->addWidget(grotesque);
    layout->addWidget(geometric);
    layout->addWidget(humanist);
    layout->addStretch(3);
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
