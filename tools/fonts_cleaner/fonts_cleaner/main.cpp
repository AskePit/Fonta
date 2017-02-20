#include <QCoreApplication>
#include <QSettings>
#include <QDebug>
#include <QFile>

int main()
{
    QSettings fontaReg("PitM", "Fonta");
    QStringList filesToDelete = fontaReg.value("FilesToDelete").toStringList();
    for(int i = 0; i<filesToDelete.count(); ++i) {
        const QString &f = filesToDelete[i];

        QFile file(f);
        bool ok = true;
        ok &= file.setPermissions(QFile::ReadOther | QFile::WriteOther);
        ok &= file.remove();

        if(ok) {
            filesToDelete.removeAt(i);
            --i;
        } else {
            qDebug() << QString("could not remove %1 file").arg(f);
        }
    }

    // files that couldn't be deleted go back to registry
    fontaReg.setValue("FilesToDelete", filesToDelete);

    return 0;
}
