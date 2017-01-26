#include "about.h"
#include "types.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

namespace fonta {

About::About(const Version& version, QWidget *parent) :
    QDialog(parent)
{
    resize(175, 85);

    auto vLayout = new QVBoxLayout(this);
    auto vSpacer3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vLayout->addItem(vSpacer3);

    auto label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter);
    vLayout->addWidget(label);

    auto vSpacer2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vLayout->addItem(vSpacer2);

    auto hLayout = new QHBoxLayout();
    auto horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hLayout->addItem(horizontalSpacer);

    auto pushButton = new QPushButton(this);
    pushButton->setObjectName(QStringLiteral("pushButton"));
    hLayout->addWidget(pushButton);

    auto hSpacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hLayout->addItem(hSpacer2);
    vLayout->addLayout(hLayout);

    auto verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vLayout->addItem(verticalSpacer);

    setWindowTitle(tr("About"));
    label->setText(tr("Fonta v. %1").arg(version.str));
    pushButton->setText(tr("OK"));

    QMetaObject::connectSlotsByName(this);
}

About::~About()
{
}

void About::on_pushButton_clicked()
{
    hide();
}

} // namespace fonta
