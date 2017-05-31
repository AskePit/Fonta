#include "filterwizard.h"

#include "fontadb.h"
#include "mainwindow.h"

#include <QApplication>
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QVariant>
#include <QRect>
#include <QUrl>
#include <QDesktopServices>

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

    MainWindow *window = dynamic_cast<MainWindow*>(parent());
    QStringList l;

    bool no_specific_serif = (!oldstyle && !transitional && !modern && !slab)
                          || ( oldstyle &&  transitional &&  modern &&  slab);

    bool no_specific_sans = (!grotesque && !geometric && !humanist)
                         || ( grotesque &&  geometric &&  humanist);

    bool no_extra_specific = !cyrillic && !monospaced;

    bool all_types = (!serif && !sans && !script && !display && !symbolic)
                  || ( serif &&  sans &&  script &&  display &&  symbolic);

    bool only_serif      =  serif && !sans && !script && !display && !symbolic;
    bool only_sans       = !serif &&  sans && !script && !display && !symbolic;
    bool only_script     = !serif && !sans &&  script && !display && !symbolic;
    bool only_display    = !serif && !sans && !script &&  display && !symbolic;
    bool only_symbolic   = !serif && !sans && !script && !display &&  symbolic;
    bool only_cyrillic   =  cyrillic && !monospaced && all_types && no_specific_serif && no_specific_sans;
    bool only_monospaced = !cyrillic &&  monospaced && all_types && no_specific_serif && no_specific_sans;

    bool not_custom = (no_specific_serif && no_specific_sans && no_extra_specific
                       && (all_types || only_serif || only_sans || only_script || only_display || only_symbolic)
                      )
                    || only_cyrillic
                    || only_monospaced;

    if(not_custom) {
        FilterMode::type mode = FilterMode::All;
        if(only_serif) {
            mode = FilterMode::Serif;
        } else if(only_sans) {
            mode = FilterMode::SansSerif;
        } else if(only_script) {
            mode = FilterMode::Script;
        } else if(only_display) {
            mode = FilterMode::Decorative;
        } else if(only_symbolic) {
            mode = FilterMode::Symbolic;
        } else if(only_cyrillic) {
            mode = FilterMode::Cyrillic;
        } else if(only_monospaced) {
            mode = FilterMode::Monospace;
        } else if(all_types) {
            mode = FilterMode::All;
        }

        window->filterFontList(l, mode);
        QDialog::accept();
        return;
    }

    if(all_types)         { serif = sans = script = display = symbolic = true; }
    if(no_specific_serif) { oldstyle = transitional = modern = slab = true; }
    if(no_specific_sans)  { grotesque = geometric = humanist = true; }

    DB& db = fontaDB();

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

    window->filterFontList(l);
    QDialog::accept();
}

static QLabel *makeLink(CStringRef htmlPath)
{
    QLabel *link = new QLabel(QString("%1 <a href=\"%2\">%3</a>.").arg(
        QApplication::translate("fonta::FilterWizard", "For detailed information"),
        htmlPath,
        QApplication::translate("fonta::FilterWizard", "click here")
    ));

    QObject::connect(link, &QLabel::linkActivated, [](CStringRef link){
        QDesktopServices::openUrl(QUrl::fromLocalFile(link));
    });

    return link;
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
    QLabel *detailsLink = makeLink("./html/fonts_guide/classes.html");

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
    layout->addWidget(detailsLink);
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
    QLabel *detailsLink = makeLink("./html/fonts_guide/serif.html");

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
    layout->addWidget(detailsLink);
    setLayout(layout);
}

int SerifFamilyPage::nextId() const
{
    bool sans = field("sans").toBool();
    return sans ? FilterWizard::Page_SansFamily : FilterWizard::Page_Finish;
}

SansFamilyPage::SansFamilyPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Choose sans families"));
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/pic/filter/intro.jpg"));

    grotesque = new QCheckBox(tr("Grotesque Sans"));
    geometric = new QCheckBox(tr("Geometric Sans"));
    humanist = new QCheckBox(tr("Humanist Sans"));
    QLabel *detailsLink = makeLink("./html/fonts_guide/sans.html");

    registerField("grotesque", grotesque);
    registerField("geometric", geometric);
    registerField("humanist", humanist);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addStretch(1);
    layout->addWidget(grotesque);
    layout->addWidget(geometric);
    layout->addWidget(humanist);
    layout->addStretch(3);
    layout->addWidget(detailsLink);
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
