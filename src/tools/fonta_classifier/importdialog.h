#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDialog>

namespace Ui {
class ImportDialog;
}

class ImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportDialog(QWidget *parent = 0);
    ~ImportDialog();

    QString fileName() const;
    int info() const;

private slots:
    void on_pushButton_clicked();

private:
    Ui::ImportDialog *ui;
};

#endif // IMPORTDIALOG_H
