#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>

class QVersionNumber;

namespace fonta {

class About : public QDialog
{
    Q_OBJECT

public:
    explicit About(const QVersionNumber& version, QWidget *parent = 0);
    ~About();

private slots:
    void on_pushButton_clicked();
};

} // namespace fonta

#endif // ABOUT_H
