#include <QSettings>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <windows.h>

using CStringRef = const QString&;
#define cauto const auto&

inline void clearLog()
{
    QFile f("log.txt");
    f.open(QIODevice::WriteOnly | QIODevice::Truncate);
    f.close();
}

inline void report(CStringRef &m) {
    QFile f("log.txt");
    f.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream out(&f);
    out << m << "\n";
    f.close();
}

int main()
{
    //clearLog();

    report("Started");
    QSettings fontaReg("PitM", "Fonta");
    QSettings winReg("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", QSettings::NativeFormat);

    QStringList files = fontaReg.value("FilesToDelete").toStringList();

    report("Reg phase");
    for(CStringRef f : files) {
        // WinAPI deletion
        report(QString("RemoveFontResourceW for %1...").arg(f));
        bool did = RemoveFontResourceW(f.toStdWString().c_str());
        if(did) {
            report(QString("RemoveFontResourceW for %1 done").arg(f));
        } else {
            report(QString("RemoveFontResourceW for %1 failed!").arg(f));
        }

        // Fonts registry cleanup
        cauto regKeys = winReg.allKeys(); // "Arial (TrueType)"="arial.ttf"

        QFileInfo info(f);
        const QString name = info.fileName();

        for(CStringRef key : regKeys) {
            const QString value = winReg.value(key).toString();
            if(QString::compare(name, value, Qt::CaseInsensitive) == 0) {
                report(QString("Try to remove %1 reg key!").arg(key));
                winReg.remove(key);
            }
        }
    }

    report("Physical deletion phase");
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
            report(QString("Could not remove %1 file!").arg(f));
        }
    }


    // files that couldn't be deleted go back to registry
    fontaReg.setValue("FilesToDelete", files);

    /*report("Broadcast changes");
    SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);*/

    report("Finished");

    return 0;
}
