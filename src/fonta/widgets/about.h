#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>

struct Version;

namespace fonta {

class About : public QDialog
{
    Q_OBJECT

public:
    explicit About(const Version& version, QWidget *parent = 0);
    ~About();

private slots:
    void on_pushButton_clicked();
};

} // namespace fonta

#endif // ABOUT_H
