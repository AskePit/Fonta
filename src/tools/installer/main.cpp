#include <QSettings>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <windows.h>

using CStringRef = const QString&;
#define cauto const auto&

#define BIN "bin/"
#define FONTA "PitM\\Fonta\\"
#define PROGRAMS "C:\\Program Files (x86)\\" FONTA
#define DATA "C:\\ProgramData\\" FONTA

#define PATH_REG "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment"
#define FONTA_EXT_REG "HKEY_CLASSES_ROOT\\.fonta"
#define EXT_HANDLER "fontafile"
#define DESC_REG "HKEY_CLASSES_ROOT\\" EXT_HANDLER
#define ICON_REG "HKEY_CLASSES_ROOT\\" EXT_HANDLER "\\DefaultIcon"
#define OPEN_REG "HKEY_CLASSES_ROOT\\" EXT_HANDLER "\\shell\\open\\command"
#define OPEN_VAL PROGRAMS "fonta.exe %1"
#define ICON_VAL PROGRAMS "icon.png"

#define SET_DEFAULT_REG(p, v) QSettings((p), QSettings::NativeFormat).setValue(".", QString(v))

static bool copyFileForced(const QString &from, const QString &to)
{
    if (QFileInfo(from) == QFileInfo(to)) {
        return true;
    }

    if (QFile::exists(to)) {
        QFile::remove(to);
    }
    return QFile::copy(from, to);
}

static bool copyRecursively(const QString &srcDir, const QString &dstDir)
{
    QFileInfo srcFileInfo(srcDir);
    if (srcFileInfo.isDir()) {

        QDir targetDir;
        if (!targetDir.mkpath(dstDir)) {
            return false;
        }
        QDir sourceDir(srcDir);
        QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
        for(const QString &fileName : fileNames) {
            const QString postfix = "/" + fileName;
            const QString newSrc = srcDir + postfix;
            const QString newDst = dstDir + postfix;
            if (!copyRecursively(newSrc, newDst)) {
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

    copyRecursively(BIN, PROGRAMS);
    QDir().mkpath(DATA);


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

    qDebug() << "Finished";

    return 0;
}
