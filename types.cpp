#include "types.h"
#include <QApplication>
#include <QDesktopWidget>


Version::Version(int major, int minor, int build)
    : major(major)
    , minor(minor)
    , build(build)
{
    const QChar dot = '.';
    str = QString::number(major);
    str += dot;
    str += QString::number(minor);
    str += dot;
    str += QString::number(build);
}

int getDPI() {
    static int dpi = QApplication::desktop()->logicalDpiX();
    return dpi;
}
