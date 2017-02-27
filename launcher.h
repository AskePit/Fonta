#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QObject>
#include "types.h"

namespace fonta {

class Launcher : public QObject
{
    Q_OBJECT
public:
    explicit Launcher(CStringRef fileToOpen = QString(), QObject *parent = 0);
    void start();

signals:

public slots:

private:
    QString m_fileToOpen;
};

} // namespace fonta

#endif // LAUNCHER_H
