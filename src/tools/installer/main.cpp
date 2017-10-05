#include <QSettings>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QStandardPaths>
#include <windows.h>

#define BIN ".\\"
#define FONTA "PitM\\Fonta\\"
#define PROGRAMS "C:\\Program Files (x86)\\" FONTA
#define DATA "C:\\ProgramData\\" FONTA
#define STARTUP "C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\" FONTA

#define PATH_REG "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment"
#define FONTA_EXT_REG "HKEY_CLASSES_ROOT\\.fonta"
#define EXT_HANDLER "fontafile"
#define DESC_REG "HKEY_CLASSES_ROOT\\" EXT_HANDLER
#define ICON_REG "HKEY_CLASSES_ROOT\\" EXT_HANDLER "\\DefaultIcon"
#define OPEN_REG "HKEY_CLASSES_ROOT\\" EXT_HANDLER "\\shell\\open\\command"
#define OPEN_VAL PROGRAMS "fonta.exe %1"
#define ICON_VAL PROGRAMS "file_icon.ico"

#define SET_DEFAULT_REG(p, v) QSettings((p), QSettings::NativeFormat).setValue(QStringLiteral("."), QString(v))

static bool copyFileForced(const QString &from, const QString &to)
{
    if (QFileInfo(from) == QFileInfo(to)) {
        return true;
    }

    if (QFile::exists(to)) {
        if(!QFile::remove(to)) {
            qWarning() << QString("Could not rewrite %1 file. Either file is used or you have no enough rights").arg(to);
            return false;
        }
    }
    return QFile::copy(from, to);
}

static bool copyRecursively(const QString &srcDir, const QString &dstDir)
{
    QFileInfo srcFileInfo(srcDir);
    if (srcFileInfo.isDir()) {

        QDir targetDir;
        if (!targetDir.mkpath(dstDir)) {
            qWarning() << QString("Could not create %1 path. Maybe you have no enough rights").arg(dstDir);
            return false;
        }
        QDir sourceDir(srcDir);
        const QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
        for(const QString &fileName : fileNames) {
            const QString postfix = QDir::separator() + fileName;
            const QString newSrc = srcDir + postfix;
            const QString newDst = dstDir + postfix;
            if (!copyRecursively(newSrc, newDst)) {
                qWarning() << QString("Copy from %1 to %2 has failed!").arg(newSrc, newDst);
                return false;
            }
        }
    } else {
        if (!copyFileForced(srcDir, dstDir)) {
            return false;
        }
    }
    return true;
}

int main()
{
    qDebug() << "Fonta installation...";

    ///////////////////////
    /// Copy stuff
    ///////////////////////

    if(!copyRecursively(QStringLiteral(BIN), QStringLiteral(PROGRAMS))) {
        qWarning() << QString("Copy from %1 to %2 has failed!").arg(QStringLiteral(BIN), QStringLiteral(PROGRAMS));
        return 0;
    }

    if(!QDir().mkpath(QStringLiteral(DATA))) {
        qWarning() << QString("Could not create %1 path. Maybe you have no enough rights").arg(QStringLiteral(DATA));
        return 0;
    }

    QFile::remove(QStringLiteral(PROGRAMS "INSTALL.exe"));


    ///////////////////////
    /// PATH
    ///////////////////////

    QSettings pathReg(QStringLiteral(PATH_REG), QSettings::NativeFormat);
    QString path = pathReg.value(QStringLiteral("Path"), QStringLiteral("")).toString();

    if(!path.contains(PROGRAMS)) {
        if(!path.endsWith(';')) {
            path += ';';
        }
        path += QStringLiteral(PROGRAMS);

        pathReg.setValue(QStringLiteral("Path"), path);
        SendMessageA(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)"Environment");
    }


    ///////////////////////
    /// .fonta ext
    ///////////////////////

    SET_DEFAULT_REG(QStringLiteral(FONTA_EXT_REG), QStringLiteral(EXT_HANDLER));
    SET_DEFAULT_REG(QStringLiteral(DESC_REG), QStringLiteral("Fonta File"));
    SET_DEFAULT_REG(QStringLiteral(ICON_REG), QStringLiteral(ICON_VAL));
    SET_DEFAULT_REG(QStringLiteral(OPEN_REG), QStringLiteral(OPEN_VAL));


    ///////////////////////
    /// links
    ///////////////////////

    QDir().mkpath(QStringLiteral(STARTUP));

    QFile fonta_bin(QStringLiteral(PROGRAMS "fonta.exe"));
    fonta_bin.link(QStringLiteral(STARTUP "Fonta.lnk"));

    auto desktops = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation);
    if(desktops.count()) {
        fonta_bin.link(desktops[0] + QLatin1String("\\Fonta.lnk"));
    }

    qDebug() << "Finished";

    return 0;
}
