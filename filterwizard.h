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
    void accept() override;
};

class GeneralPage : public QWizardPage
{
    Q_OBJECT

public:
    GeneralPage(QWidget *parent = 0);
    int nextId() const override;

private:
    QCheckBox* serifButton;
    QCheckBox* sansButton;
    QCheckBox* scriptButton;
    QCheckBox* displayButton;
    QCheckBox* symbolicButton;
};


class SerifFamilyPage : public QWizardPage
{
    Q_OBJECT

public:
    SerifFamilyPage(QWidget *parent = 0);
    int nextId() const override;

private:
    QCheckBox* oldstyle;
    QCheckBox* transitional;
    QCheckBox* modern;
    QCheckBox* slab;
};

class SansFamilyPage : public QWizardPage
{
    Q_OBJECT

public:
    SansFamilyPage(QWidget *parent = 0);
    int nextId() const override;

private:
    QCheckBox* grotesque;
    QCheckBox* geometric;
    QCheckBox* humanist;
};

class FinishPage : public QWizardPage
{
    Q_OBJECT

public:
    FinishPage(QWidget *parent = 0);
    int nextId() const override;

private:
    QCheckBox* cyrillicBox;
    QCheckBox* monospacedBox;
};

} // namespace fonta

#endif // FILTERWIZARD_H
