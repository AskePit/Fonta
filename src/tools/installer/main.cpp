#include <QSettings>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QStandardPaths>
#include <windows.h>

#define BIN "bin/"
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

#define SET_DEFAULT_REG(p, v) QSettings((p), QSettings::NativeFormat).setValue(".", QString(v))

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
        QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
        for(const QString &fileName : fileNames) {
            const QString postfix = "/" + fileName;
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

    if(!copyRecursively(BIN, PROGRAMS)) {
        qWarning() << QString("Copy from %1 to %2 has failed!").arg(BIN, PROGRAMS);
        return 0;
    }

    if(!QDir().mkpath(DATA)) {
        qWarning() << QString("Could not create %1 path. Maybe you have no enough rights").arg(DATA);
        return 0;
    }


    ///////////////////////
    /// PATH
    ///////////////////////

    QSettings pathReg(PATH_REG, QSettings::NativeFormat);
    QString path = pathReg.value("Path", "").toString();

    if(!path.contains(PROGRAMS)) {
        if(!path.endsWith(';')) {
            path += ';';
        }
        path += PROGRAMS;

        pathReg.setValue("Path", path);
        SendMessageA(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)"Environment");
    }


    ///////////////////////
    /// .fonta ext
    ///////////////////////

    SET_DEFAULT_REG(FONTA_EXT_REG, EXT_HANDLER);
    SET_DEFAULT_REG(DESC_REG, "Fonta File");
    SET_DEFAULT_REG(ICON_REG, ICON_VAL);
    SET_DEFAULT_REG(OPEN_REG, OPEN_VAL);


    ///////////////////////
    /// links
    ///////////////////////

    QDir().mkpath(STARTUP);

    QFile fonta_bin(PROGRAMS "fonta.exe");
    fonta_bin.link(STARTUP "Fonta.lnk");

    auto desktops = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation);
    if(desktops.count()) {
        fonta_bin.link(desktops[0] + "\\Fonta.lnk");
    }

    qDebug() << "Finished";

    return 0;
}
