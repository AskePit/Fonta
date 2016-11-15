#include "filterwizard.h"

#include "fontadb.h"
#include "fontawindow.h"

#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QVariant>
#include <QRect>

FilterWizard::FilterWizard(QWidget *parent)
    : QWizard(parent)
{
    setPage(Page_General, new GeneralPage);
    setPage(Page_SerifFamily, new SerifFamilyPage);
    setPage(Page_SerifStyle, new SerifStylePage);
    setPage(Page_SansFamily, new SansFamilyPage);
    setPage(Page_SansStyle, new SansStylePage);
    setPage(Page_Finish, new FinishPage);

    setStartId(Page_General);
    setWizardStyle(ModernStyle);

    setWindowTitle("Filter Wizard");

    QRect parentG = dynamic_cast<FontaWindow*>(parent)->geometry();
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

    declBool(extSerif);
    declBool(extSans);

    declBool(oldstyle);
    declBool(transitional);
    declBool(modern);
    declBool(slab);

    declBool(cove);
    declBool(square);
    declBool(bone);
    declBool(asymmetric);
    declBool(triangle);

    declBool(grotesque);
    declBool(geometric);
    declBool(humanist);

    declBool(normal);
    declBool(rounded);
    declBool(flarred);

    declBool(monospaced);
    declBool(cyrillic);

#undef declBool

    if(!serif && !sans && !script && !display && !symbolic) {
        serif = sans = script = display = symbolic = true;
    }

    if(!oldstyle && !transitional && !modern && !slab) {
        oldstyle = transitional = modern = slab = true;
    }

    if(!cove && !square && !bone && !asymmetric && !triangle) {
        cove = square = bone = asymmetric = triangle = true;
    }

    if(!grotesque && !geometric && !humanist) {
        grotesque = geometric = humanist = true;
    }

    if(!normal && !rounded && !flarred) {
        normal = rounded = flarred = true;
    }

    FontaDB& db = fontaDB();

    QStringList l;
    for (CStringRef f : db.families()) {
        if(monospaced && !db.isMonospaced(f)) { continue; }
        if(cyrillic && !db.isCyrillic(f)) { continue; }

        if(serif) {
            if(extSerif) {
                bool ok = false;
                ok |= oldstyle && db.isOldStyle(f);
                ok |= transitional && db.isTransitional(f);
                ok |= modern && db.isModern(f);
                ok |= slab && db.isSlab(f);
                if(!ok) { goto sans_lbl; }

                ok = false;
                ok |= cove && db.isCoveSerif(f);
                ok |= square && db.isSquareSerif(f);
                ok |= bone && db.isBoneSerif(f);
                ok |= asymmetric && db.isAsymmetricSerif(f);
                ok |= triangle && db.isTriangleSerif(f);
                if(ok) {
                    l << f;
                    continue;
                }
            } else {
                if(db.isSerif(f)) {
                    l << f;
                    continue;
                }
            }
        }

    sans_lbl:
        if(sans) {
            if(extSans) {
                bool ok = false;
                ok |= grotesque && db.isGrotesque(f);
                ok |= geometric && db.isGeometric(f);
                ok |= humanist && db.isHumanist(f);
                if(!ok) { goto script_lbl; }

                ok = false;
                ok |= normal && db.isNormalSans(f);
                ok |= rounded && db.isRoundedSans(f);
                ok |= flarred && db.isFlarredSans(f);
                if(ok) {
                    l << f;
                    continue;
                }
            } else {
                if(db.isSansSerif(f)) {
                    l << f;
                    continue;
                }
            }
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

    dynamic_cast<FontaWindow*>(parent())->filterFontList(l);
    QDialog::accept();
}

void GeneralPage::addGeneralBloc(QPushButton** button, QCheckBox** box, int width, int height, CStringRef blockName, CStringRef extBlocName)
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

    if(box == 0) {
        return;
    }

    (*box) = new QCheckBox("", *button);
    (*box)->setEnabled(false);
    (*box)->setGeometry(10, height-(*box)->height(), (*box)->width()-20, (*box)->height());
    (*box)->hide();
    (*box)->setStyleSheet(
        "QCheckBox{"
          "color: #e4ccac;"
          "font-family: \"Arial Narrow\";"
        "}"
        "QCheckBox::indicator{"
          "border: 3px no;"
          "background-color: #e4ccac;"
        "}"
        "QCheckBox::indicator:on{"
          "border: 2px solid #e4ccac;"
          "background-color: #5a5a5a;"
        "}"
    );


    registerField(extBlocName, *box);

    connect(*button, &QPushButton::toggled, *box, &QCheckBox::setEnabled);
    connect(*button, &QPushButton::toggled, *box, &QCheckBox::setVisible);
}

GeneralPage::GeneralPage(QWidget *parent)
    : QWizardPage(parent)
{
    addGeneralBloc(&serifButton, &extSerifBox, 210, 144, "serif", "extSerif");
    addGeneralBloc(&sansButton, &extSansBox, 209, 144, "sans", "extSans");
    addGeneralBloc(&scriptButton, 0, 170, 171, "script", "");
    addGeneralBloc(&displayButton, 0, 166, 171, "display", "");
    addGeneralBloc(&symbolicButton, 0, 83, 171, "symbolic", "");

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

    QLabel* descr = new QLabel("\nChoose font categories. For advanced filtering toogle the checkbox.");
    descr->setStyleSheet("qproperty-alignment: AlignCenter;");
    layout->addWidget(descr);

    layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(layout);
}

int GeneralPage::nextId() const
{
    bool serif = field("serif").toBool();
    bool extSerif = field("extSerif").toBool();

    bool sans = field("sans").toBool();
    bool extSans = field("extSans").toBool();

    if(serif && extSerif) {
        return FilterWizard::Page_SerifFamily;
    } else if(sans && extSans) {
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

    QLabel* descr = new QLabel("\nChoose serif family.");
    descr->setStyleSheet("qproperty-alignment: AlignCenter;");
    layout->addWidget(descr);

    layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(layout);
}

int SerifFamilyPage::nextId() const
{
    return FilterWizard::Page_SerifStyle;
}

void SerifStylePage::addGeneralBloc(QPushButton** button, int width, int height, CStringRef blockName)
{
    *button = new QPushButton();
    (*button)->setCheckable(true);
    (*button)->setStyleSheet(
        "QPushButton{"
          "background-image: url(:/pic/filter/filter_buttons/serif_serifs/" + blockName + "_inactive.png);"
          "width: " + QString::number(width) + ";"
          "height: " + QString::number(height) + ";"
          "max-width: " + QString::number(width) + ";"
          "max-height: " + QString::number(height) + ";"
          "border: none;"
        "}"
        "QPushButton::hover  {background-image: url(:/pic/filter/filter_buttons/serif_serifs/" + blockName + "_hover.png);}"
        "QPushButton::checked{background-image: url(:/pic/filter/filter_buttons/serif_serifs/" + blockName + "_active.png);}"
    );

    registerField(blockName, *button);
}

SerifStylePage::SerifStylePage(QWidget *parent)
    : QWizardPage(parent)
{
    addGeneralBloc(&cove, 210, 148, "cove");
    addGeneralBloc(&square, 209, 148, "square");
    addGeneralBloc(&bone, 140, 167, "bone");
    addGeneralBloc(&asymmetric, 143, 167, "asymmetric");
    addGeneralBloc(&triangle, 136, 167, "triangle");

    QHBoxLayout* hor1Layout = new QHBoxLayout;
    hor1Layout->setSpacing(0);
    hor1Layout->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout* hor2Layout = new QHBoxLayout;
    hor2Layout->setSpacing(0);
    hor2Layout->setContentsMargins(0, 0, 0, 0);

    hor1Layout->addWidget(cove);
    hor1Layout->addWidget(square);
    hor2Layout->addWidget(bone);
    hor2Layout->addWidget(asymmetric);
    hor2Layout->addWidget(triangle);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(hor1Layout);
    layout->addLayout(hor2Layout);

    QLabel* descr = new QLabel("\nChoose serif style.");
    descr->setStyleSheet("qproperty-alignment: AlignCenter;");
    layout->addWidget(descr);

    layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(layout);
}

int SerifStylePage::nextId() const
{
    bool sans = field("sans").toBool();
    bool extSans = field("extSans").toBool();

    if(sans && extSans) {
        return FilterWizard::Page_SansFamily;
    }

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

    QLabel* descr = new QLabel("\nChoose sans family.");
    descr->setStyleSheet("qproperty-alignment: AlignCenter;");
    layout->addWidget(descr);

    layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(layout);
}

int SansFamilyPage::nextId() const
{
    return FilterWizard::Page_SansStyle;
}

void SansStylePage::addGeneralBloc(QPushButton** button, int width, int height, CStringRef blockName)
{
    *button = new QPushButton();
    (*button)->setCheckable(true);
    (*button)->setStyleSheet(
        "QPushButton{"
          "background-image: url(:/pic/filter/filter_buttons/sans_serifs/" + blockName + "_inactive.png);"
          "width: " + QString::number(width) + ";"
          "height: " + QString::number(height) + ";"
          "max-width: " + QString::number(width) + ";"
          "max-height: " + QString::number(height) + ";"
          "border: none;"
        "}"
        "QPushButton::hover  {background-image: url(:/pic/filter/filter_buttons/sans_serifs/" + blockName + "_hover.png);}"
        "QPushButton::checked{background-image: url(:/pic/filter/filter_buttons/sans_serifs/" + blockName + "_active.png);}"
    );

    registerField(blockName, *button);
}

SansStylePage::SansStylePage(QWidget *parent)
    : QWizardPage(parent)
{
    addGeneralBloc(&normal, 419, 150, "normal");
    addGeneralBloc(&rounded, 210, 165, "rounded");
    addGeneralBloc(&flarred, 209, 165, "flarred");

    QHBoxLayout* hor1Layout = new QHBoxLayout;
    hor1Layout->setSpacing(0);
    hor1Layout->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout* hor2Layout = new QHBoxLayout;
    hor2Layout->setSpacing(0);
    hor2Layout->setContentsMargins(0, 0, 0, 0);

    hor1Layout->addWidget(normal);
    hor2Layout->addWidget(rounded);
    hor2Layout->addWidget(flarred);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(hor1Layout);
    layout->addLayout(hor2Layout);

    QLabel* descr = new QLabel("\nChoose sans style.");
    descr->setStyleSheet("qproperty-alignment: AlignCenter;");
    layout->addWidget(descr);

    layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(layout);
}

int SansStylePage::nextId() const
{
    return FilterWizard::Page_Finish;
}


FinishPage::FinishPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle("Last step...");
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/pic/filter/intro.jpg"));

    QLabel* topLabel = new QLabel("Specify additional options if you wish.\nHope you'll be satisfied with results!");

    cyrillicBox = new QCheckBox("Cyrillic support");
    monospacedBox = new QCheckBox("Monospaced");

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

