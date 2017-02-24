#ifndef FILTERWIZARD_H
#define FILTERWIZARD_H

#include <QWizard>
#include "types.h"

class QCheckBox;
class QPushButton;
class QLabel;
class QGridLayout;

namespace fonta {

class FilterWizard : public QWizard
{
    Q_OBJECT

public:
    enum { Page_General, Page_SerifFamily, Page_SansFamily, Page_Finish };

    FilterWizard(QWidget *parent = 0);
    void accept() Q_DECL_OVERRIDE;
};

class GeneralPage : public QWizardPage
{
    Q_OBJECT

public:
    GeneralPage(QWidget *parent = 0);
    int nextId() const Q_DECL_OVERRIDE;

private:
    QPushButton* serifButton;
    QPushButton* sansButton;
    QPushButton* scriptButton;
    QPushButton* displayButton;
    QPushButton* symbolicButton;

    void addGeneralBloc(QPushButton** button, int width, int height, CStringRef blockName);
};


class SerifFamilyPage : public QWizardPage
{
    Q_OBJECT

public:
    SerifFamilyPage(QWidget *parent = 0);
    int nextId() const Q_DECL_OVERRIDE;

private:
    QPushButton* oldstyle;
    QPushButton* transitional;
    QPushButton* modern;
    QPushButton* slab;

    void addGeneralBloc(QPushButton** button, int width, int height, CStringRef blockName);
};

class SansFamilyPage : public QWizardPage
{
    Q_OBJECT

public:
    SansFamilyPage(QWidget *parent = 0);
    int nextId() const Q_DECL_OVERRIDE;

private:
    QPushButton* grotesque;
    QPushButton* geometric;
    QPushButton* humanist;

    void addGeneralBloc(QPushButton** button, int width, int height, CStringRef blockName);
};

class FinishPage : public QWizardPage
{
    Q_OBJECT

public:
    FinishPage(QWidget *parent = 0);
    int nextId() const Q_DECL_OVERRIDE;

private:
    QCheckBox* cyrillicBox;
    QCheckBox* monospacedBox;
};

} // namespace fonta

#endif // FILTERWIZARD_H
