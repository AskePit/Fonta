#include <QSettings>
#include <QFile>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDebug>
#include <windows.h>

using CStringRef = const QString&;
#define cauto const auto&

inline void clearLog()
{
    QFile f(QStringLiteral("log.txt"));
    f.open(QIODevice::WriteOnly | QIODevice::Truncate);
    f.close();
}

inline void report(CStringRef &m) {
    QFile f(QStringLiteral("log.txt"));
    f.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream out(&f);
    out << m << QStringLiteral("\n");
    f.close();
}

int main()
{
    //clearLog();

    report(QCoreApplication::translate("GLOBAL", "Started"));
    QSettings fontaReg(QStringLiteral("PitM"), QStringLiteral("Fonta"));
    QSettings winReg(QStringLiteral("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts"), QSettings::NativeFormat);

    QStringList files = fontaReg.value(QStringLiteral("FilesToDelete")).toStringList();

    report(QCoreApplication::translate("GLOBAL", "Reg phase"));
    for(CStringRef f : std::as_const(files)) {
        // WinAPI deletion
        report(QCoreApplication::translate("GLOBAL", "RemoveFontResourceW for %1...").arg(f));
        bool did = RemoveFontResourceW(f.toStdWString().c_str());
        if(did) {
            report(QCoreApplication::translate("GLOBAL", "RemoveFontResourceW for %1 done").arg(f));
        } else {
            report(QCoreApplication::translate("GLOBAL", "RemoveFontResourceW for %1 failed!").arg(f));
        }

        // Fonts registry cleanup
        cauto regKeys = winReg.allKeys(); // "Arial (TrueType)"="arial.ttf"

        QFileInfo info(f);
        const QString name = info.fileName();

        for(CStringRef key : regKeys) {
            const QString value = winReg.value(key).toString();
            if(QString::compare(name, value, Qt::CaseInsensitive) == 0) {
                report(QCoreApplication::translate("GLOBAL", "Try to remove %1 reg key!").arg(key));
                winReg.remove(key);
            }
        }
    }

    report(QCoreApplication::translate("GLOBAL", "Physical deletion phase"));
    // Physical files deletion
    for(int i = 0; i<files.count(); ++i) {
        const QString &f = files[i];

        QFile file(f);
        bool ok = true;
        ok &= file.setPermissions(QFile::ReadOther | QFile::WriteOther);
        ok &= file.remove();

        if(ok) {
            files.removeAt(i);
            --i;
        } else {
            report(QCoreApplication::translate("GLOBAL", "Could not remove %1 file!").arg(f));
        }
    }


    // files that couldn't be deleted go back to registry
    fontaReg.setValue(QStringLiteral("FilesToDelete"), files);

    /*report(QCoreApplication::translate("GLOBAL", "Broadcast changes"));
    SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);*/

    report(QCoreApplication::translate("GLOBAL", "Finished"));

    return 0;
}
