#include "importdialog.h"
#include "ui_importdialog.h"

#include "classifier.h"

#include <QFileDialog>

ImportDialog::ImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportDialog)
{
    ui->setupUi(this);

    for(cauto type : fonta::FontType::enumerate()) {
        ui->comboBox->addItem(fonta::FontType::name(type), type);
    }
}

ImportDialog::~ImportDialog()
{
    delete ui;
}

void ImportDialog::on_pushButton_clicked()
{
    ui->lineEdit->setText(QFileDialog::getOpenFileName(this, "Select import file", "."));
}

QString ImportDialog::fileName() const
{
    return ui->lineEdit->text();
}

int ImportDialog::info() const
{
    return ui->comboBox->currentData().toInt();
}
