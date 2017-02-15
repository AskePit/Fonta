#include "FontaDB.h"

#include <QDirIterator>
#include <QTextCodec>
#include <thread>

#ifdef FONTA_MEASURES
#include <QElapsedTimer>
#include <QDebug>
#endif

#ifdef FONTA_DETAILED_DEBUG
#include <QDebug>
#endif

#include <QtEndian>
#include <QStandardPaths>
#include <QSettings>
#include <mutex>

namespace fonta {

static void getFontFiles(QStringList &out)
{
    cauto fontsDirs = QStandardPaths::standardLocations(QStandardPaths::FontsLocation);
    for(cauto dir : fontsDirs) {
        QDirIterator it(dir, {"*.ttf", "*.otf", "*.ttc", "*.otc", "*.fon"} , QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            it.next();
            QString fileName = it.filePath();

            // if file is planned tobe deleted - do not include it to list of font files
            QSettings fontaReg("PitM", "Fonta");
            QStringList filesToDelete = fontaReg.value("FilesToDelete").toStringList();
            if(!filesToDelete.contains(fileName)) {
                out << it.filePath();
            }
        }
    }
}

static inline u16 swap16(u16 x) {
    return qFromBigEndian<u16>(x);
}

static inline u32 swap32(u32 x) {
    return qFromBigEndian<u32>(x);
}

#pragma pack(push, 1)

struct TTFOffsetTable {
    char TableName[4];
    u32 CheckSum;
    u32 Offset;
    u32 Length;
};

struct TTFNameHeader {
    u16 Selector;
    u16 RecordsCount;
    u16 StorageOffset;
};

struct TTFNameRecord {
    u16 PlatformID;
    u16 EncodingID;
    u16 LanguageID;
    u16 NameID;
    u16 StringLength;
    u16 StringOffset; //from start of storage area

    TTFNameRecord() : PlatformID(1), EncodingID(0), LanguageID(0), NameID(1), StringLength(0), StringOffset(0)
    {}
};

struct TTFOS2Header {
    u32 placeholder0;
    u32 placeholder1;
    u32 placeholder2;
    u32 placeholder3;
    u32 placeholder4;
    u32 placeholder5;
    u32 placeholder6;
    u16 placeholder7;

    i16 FamilyClass;
    Panose panose;
    u32 UnicodeRange1;
};

#pragma pack(pop)

namespace TTFTable {
    enum type {
        NO = -1,
        NAME,
        OS2,
        count
    };
}

// Forwards
static QString decodeFontName(u16 platformID, u16 encodingID, const QByteArray &nameBytes);
static void readFont(TTFOffsetTable tablesMap[], QFile &f, TTFMap &TTFs, File2FontsMap &File2Fonts);
static void readTTF(QFile &f, TTFMap &TTFs, File2FontsMap &File2Fonts);
static void readTTC(QFile &f, TTFMap &TTFs, File2FontsMap &File2Fonts);
static void readFON(QFile &f, TTFMap &TTFs, File2FontsMap &File2Fonts);
static void readFontFile(CStringRef fileName, TTFMap &TTFs, File2FontsMap &File2Fonts);
static bool readTablesMap(QFile &f, TTFOffsetTable tablesMap[]);

static std::mutex readTTFMutex;
static std::mutex readFile2Fonts;


static void readFontFile(CStringRef fileName, TTFMap &TTFs, File2FontsMap &File2Fonts)
{
#ifdef FONTA_DETAILED_DEBUG
    qDebug() << qPrintable(QFileInfo(fileName).fileName()) << ":";
#endif

    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open!";
        return;
    }

    std::function<void(QFile &, QHash<QString, TTF> &, File2FontsMap &)> func;

    if(fileName.endsWith(".ttc", Qt::CaseInsensitive)) {
        func = readTTC;
    } else if(fileName.endsWith(".fon", Qt::CaseInsensitive)) {
        func = readFON;
    } else {
        func = readTTF;
    }

    func(f, TTFs, File2Fonts);
}

static void readTTC(QFile &f, TTFMap &TTFs, File2FontsMap &File2Fonts)
{
    f.seek(8);

    u32 offsetTablesCount;
    f.read((char*)&offsetTablesCount, 4);
    offsetTablesCount = swap32(offsetTablesCount);

    std::vector<u32> offsets(offsetTablesCount);
    for(u32 i = 0; i<offsetTablesCount; ++i) {
        u32 &offset = offsets[i];
        f.read((char*)&offset, 4);
        offset = swap32(offset);
    }

    for(u32 i = 0; i<offsetTablesCount; ++i) {
        f.seek(offsets[i] + 4);

        u16 fontTablesCount;
        f.read((char*)&fontTablesCount, 2);
        fontTablesCount = swap16(fontTablesCount);

        f.seek(offsets[i] + 12);
        TTFOffsetTable tablesMap[TTFTable::count];
        int tablesCount = 0;
        for(int j = 0; j<fontTablesCount; ++j) {
            tablesCount += (int)readTablesMap(f, tablesMap);
            if(tablesCount == TTFTable::count) {
                break;
            }
        }

        if(tablesCount != TTFTable::count) {
            qWarning() << "no necessary tables!";
            return;
        }

        readFont(tablesMap, f, TTFs, File2Fonts);
    }
}

static void readFON(QFile &f, TTFMap &TTFs, File2FontsMap &File2Fonts)
{
    QByteArray ba = f.readAll();

    int ibeg = ba.indexOf("FONTRES");
    if(ibeg == -1) return;

    ibeg = ba.indexOf(':', ibeg);
    if(ibeg == -1) return;
    ++ibeg;

    int iend = ba.indexOf('\0', ibeg);
    if(iend == -1) return;

    QString s(ba.mid(ibeg, iend-ibeg));

    int ipareth = s.indexOf('(');
    int icomma = s.indexOf(',');

    QString fontName;

    if(icomma == -1 && ipareth != -1) {
        s.truncate(ipareth);
    } else if(icomma != -1) {
        for(int i = icomma-1; i>=0; --i) {
            if(!s[i].isDigit()) {
                s.truncate(i+1);
                break;
            }
        }
    }

    int i = s.indexOf("Font for ");
    if(i != -1) {
        s.truncate(i);
    }

    i = s.indexOf(" Font ");
    if(i != -1) {
        s.truncate(i);
    }

    i = s.indexOf(" for ");
    if(i != -1) {
        s.truncate(i);
    }

    fontName = s.trimmed();

#ifdef FONTA_DETAILED_DEBUG
    qDebug() << '\t' << fontName;
#endif

    CStringRef fileName = f.fileName();

    {
        std::lock_guard<std::mutex> lock(readFile2Fonts);
        File2Fonts[fileName] << fontName;
        (void)lock;
    }

    {
        std::lock_guard<std::mutex> lock(readTTFMutex);
        if(TTFs.contains(fontName)) {
            TTFs[fontName].files << f.fileName();
            return;
        }
        (void)lock;
    }

    TTF ttf;
    ttf.files << fileName;

    {
        std::lock_guard<std::mutex> lock(readTTFMutex);
        TTFs[fontName] = ttf;
        (void)lock;
    }
}

static void readTTF(QFile &f, TTFMap &TTFs, File2FontsMap &File2Fonts)
{
    f.seek(12);

    TTFOffsetTable tablesMap[TTFTable::count];

    int tablesCount = 0;
    while(!f.atEnd()) {
        tablesCount += (int)readTablesMap(f, tablesMap);
        if(tablesCount == TTFTable::count) {
            break;
        }
    }

    if(tablesCount != TTFTable::count) {
        qWarning() << "no necessary tables!";
        return;
    }

    readFont(tablesMap, f, TTFs, File2Fonts);
}

static bool readTablesMap(QFile &f, TTFOffsetTable tablesMap[])
{
    TTFOffsetTable offsetTable;
    f.read((char*)&offsetTable, sizeof(TTFOffsetTable));

    TTFTable::type tableType = TTFTable::NO;

    if(memcmp(offsetTable.TableName, "name", 4) == 0) {
        tableType = TTFTable::NAME;
    }

    if(memcmp(offsetTable.TableName, "OS/2", 4) == 0) {
        tableType = TTFTable::OS2;
    }

    if(tableType != TTFTable::NO) {
        offsetTable.Length = swap32(offsetTable.Length);
        offsetTable.Offset = swap32(offsetTable.Offset);
        tablesMap[tableType] = offsetTable;
        return true;
    } else {
        return false;
    }
}

static inline u16 getU16(const unsigned char *p)
{
    u16 val;
    val = *p++ << 8;
    val |= *p;

    return val;
}

static QString getEnglishName(const uchar *table, u32 bytes)
{
    QString i18n_name;
    enum {
        NameRecordSize = 12,
        FamilyId = 1,
        MS_LangIdEnglish = 0x009
    };

    // get the name table
    quint16 count;
    quint16 string_offset;
    const unsigned char *names;

    int microsoft_id = -1;
    int apple_id = -1;
    int unicode_id = -1;

    if(getU16(table) != 0)
        goto error;

    count = getU16(table+2);
    string_offset = getU16(table+4);
    names = table + 6;

    if(string_offset >= bytes || 6 + count*NameRecordSize > string_offset)
        goto error;

    for(int i = 0; i < count; ++i) {
        // search for the correct name entry

        u16 platform_id = getU16(names + i*NameRecordSize);
        u16 encoding_id = getU16(names + 2 + i*NameRecordSize);
        u16 language_id = getU16(names + 4 + i*NameRecordSize);
        u16 name_id = getU16(names + 6 + i*NameRecordSize);

        if(name_id != FamilyId)
            continue;

        enum {
            PlatformId_Unicode = 0,
            PlatformId_Apple = 1,
            PlatformId_Microsoft = 3
        };

        u16 length = getU16(names + 8 + i*NameRecordSize);
        u16 offset = getU16(names + 10 + i*NameRecordSize);
        if(u32(string_offset + offset + length) >= bytes)
            continue;

        if ((platform_id == PlatformId_Microsoft
            && (encoding_id == 0 || encoding_id == 1))
            && (language_id & 0x3ff) == MS_LangIdEnglish
            && microsoft_id == -1)
            microsoft_id = i;
            // not sure if encoding id 4 for Unicode is utf16 or ucs4...
        else if(platform_id == PlatformId_Unicode && encoding_id < 4 && unicode_id == -1)
            unicode_id = i;
        else if(platform_id == PlatformId_Apple && encoding_id == 0 && language_id == 0)
            apple_id = i;
    }
    {
        bool unicode = false;
        int id = -1;
        if(microsoft_id != -1) {
            id = microsoft_id;
            unicode = true;
        } else if(apple_id != -1) {
            id = apple_id;
            unicode = false;
        } else if (unicode_id != -1) {
            id = unicode_id;
            unicode = true;
        }
        if(id != -1) {
            u16 length = getU16(names + 8 + id*NameRecordSize);
            u16 offset = getU16(names + 10 + id*NameRecordSize);
            if(unicode) {
                // utf16

                length /= 2;
                i18n_name.resize(length);
                QChar *uc = (QChar *) i18n_name.unicode();
                const unsigned char *string = table + string_offset + offset;
                for(int i = 0; i < length; ++i)
                    uc[i] = getU16(string + 2*i);
            } else {
                // Apple Roman

                i18n_name.resize(length);
                QChar *uc = (QChar *) i18n_name.unicode();
                const unsigned char *string = table + string_offset + offset;
                for(int i = 0; i < length; ++i)
                    uc[i] = QLatin1Char(string[i]);
            }
        }
    }
  error:
    //qDebug("got i18n name of '%s' for font '%s'", i18n_name.latin1(), familyName.toLocal8Bit().data());
    return i18n_name;
}

static void readFont(TTFOffsetTable tablesMap[], QFile &f, TTFMap &TTFs, File2FontsMap &File2Fonts)
{
    /////////
    // name
    ///////
    TTFOffsetTable &nameOffsetTable = tablesMap[TTFTable::NAME];
    f.seek(nameOffsetTable.Offset);

    TTFNameHeader nameHeader;
    f.read((char*)&nameHeader, sizeof(TTFNameHeader));
    nameHeader.RecordsCount = swap16(nameHeader.RecordsCount);
    nameHeader.StorageOffset = swap16(nameHeader.StorageOffset);

    TTFNameRecord nameRecord;
    TTFNameRecord lastNameRecord;
    QByteArray nameBytes;
    QString fontName;
    bool properLanguage = false; // english-like language

    enum {
        NameRecordSize = 12,
        FamilyId = 1,
        MS_LangIdEnglish = 0x009
    };

    // get the name table
    quint16 count;
    quint16 string_offset;
    const unsigned char *names;

    int microsoft_id = -1;
    int apple_id = -1;
    int unicode_id = -1;

    for(int i = 0; i<nameHeader.RecordsCount; ++i) {
        f.read((char*)&nameRecord, sizeof(TTFNameRecord));
        nameRecord.NameID = swap16(nameRecord.NameID);

        if(nameRecord.NameID != FamilyId) {
            continue;
        }

        nameRecord.PlatformID = swap16(nameRecord.PlatformID);
        nameRecord.EncodingID = swap16(nameRecord.EncodingID);
        nameRecord.StringLength = swap16(nameRecord.StringLength);
        nameRecord.StringOffset = swap16(nameRecord.StringOffset);

        enum {
            PlatformId_Unicode = 0,
            PlatformId_Apple = 1,
            PlatformId_Microsoft = 3
        };

        // save file position, so we can return to continue with search
        quint64 nPos = f.pos();
        f.seek(nameOffsetTable.Offset + nameHeader.StorageOffset + nameRecord.StringOffset);

        nameBytes = f.read(nameRecord.StringLength);

        properLanguage = false;

        u8 langCode = nameRecord.LanguageID >> 8; // notice that we did not do swapU16 on it!
        /*if(nameRecord.PlatformID == 1) {
            properLanguage = langCode <= 9; // English, French, German, Italian, Dutch, Swedish, Spanish, Danish, Portuguese, Norwegian
        } else*/ if(nameRecord.PlatformID == 3) {
            properLanguage = langCode == 0x09 || // English
                             langCode == 0x07 || // German
                             langCode == 0x0C || // French
                             langCode == 0x0A || // Spanish
                             langCode == 0x3B;   // Scandinavic
        }

        if(properLanguage) {
            fontName = decodeFontName(nameRecord.PlatformID, nameRecord.EncodingID, nameBytes);
#ifdef FONTA_DETAILED_DEBUG
            qDebug() << '\t' << nameRecord.PlatformID << nameRecord.EncodingID << langCode << fontName;
#endif
            break;
        } else {
            lastNameRecord = nameRecord;
        }

        f.seek(nPos);
    }

    if(!properLanguage) {
        // use last record to extract font name
        fontName = decodeFontName(lastNameRecord.PlatformID, lastNameRecord.EncodingID, nameBytes);
#ifdef FONTA_DETAILED_DEBUG
        qDebug() << '\t' << "not proper!" << lastNameRecord.PlatformID << lastNameRecord.EncodingID << (lastNameRecord.LanguageID>>8) << fontName;
#endif
    }

    CStringRef fileName = f.fileName();

    {
        std::lock_guard<std::mutex> lock(readFile2Fonts);
        File2Fonts[fileName] << fontName;
        (void)lock;
    }

    {
        std::lock_guard<std::mutex> lock(readTTFMutex);
        if(TTFs.contains(fontName)) {
            TTFs[fontName].files << fileName;
            return;
        }
        (void)lock;
    }

    TTF ttf;
    ttf.files << fileName;
    //qDebug() << '\t' << fontName;

    /////////
    // OS/2
    ///////
    TTFOffsetTable& os2OffsetTable = tablesMap[TTFTable::OS2];
    f.seek(os2OffsetTable.Offset);

    TTFOS2Header os2Header;
    f.read((char*)&os2Header, sizeof(TTFOS2Header));
    os2Header.FamilyClass = swap16(os2Header.FamilyClass);
    os2Header.UnicodeRange1 = swap32(os2Header.UnicodeRange1);

    ttf.panose = os2Header.panose;
    ttf.familyClass = (FamilyClass::type)(os2Header.FamilyClass >> 8);
    ttf.familySubClass = (int)(os2Header.FamilyClass & 0xFF);


    cauto langBit = [&os2Header](int bit) {
        return !!(os2Header.UnicodeRange1 & (1<<bit));
    };
    ttf.latin = langBit(0) || langBit(1) || langBit(2) || langBit(3);
    ttf.cyrillic = langBit(9);

    {
        std::lock_guard<std::mutex> lock(readTTFMutex);
        TTFs[fontName] = ttf;
        (void)lock;
    }
}

static QString decodeFontName(u16 platformID, u16 encodingID, const QByteArray &nameBytes)
{
    u16 code = (platformID<<8) + encodingID;
    switch(code) {
        case 0x0000:
        case 0x0003:
        case 0x0300:
        case 0x0302:
        case 0x030A:
        case 0x0301: return QTextCodec::codecForMib(1013)->toUnicode(nameBytes); break;

        case 0x0100:
        default:     return QTextCodec::codecForMib(106)->toUnicode(nameBytes); break;
    }
}

#ifndef FONTA_DETAILED_DEBUG

static void loadTTFChunk(const QStringList &out, int from, int to, TTFMap &TTFs, File2FontsMap &File2Fonts)
{
    for(int i = from; i<=to; ++i) {
        readFontFile(out[i], TTFs, File2Fonts);
    }
}
#endif

DB::DB()
{
    // QFontDatabase seems to use all font files from C:/Windows/Fonts.
    // It's impossible todelete any file while programm is running.
    // Solution: read files to be deleted from registry at Fonta
    // startup exactly before QFontDatabase creation and try to delete them.
    {
        QSettings fontaReg("PitM", "Fonta");
        QStringList filesToDelete = fontaReg.value("FilesToDelete").toStringList();
        for(int i = 0; i<filesToDelete.count(); ++i) {
            CStringRef f = filesToDelete[i];

            QFile file(f);
            bool ok = true;
            ok &= file.setPermissions(QFile::ReadOther | QFile::WriteOther);
            ok &= file.remove();

            if(ok) {
                filesToDelete.removeAt(i);
                --i;
            }
        }

        // files that couldn't be deleted go back to registry
        fontaReg.setValue("FilesToDelete", filesToDelete);
    }

    QtDB = new QFontDatabase;

    QStringList out;
    getFontFiles(out);

#ifdef FONTA_MEASURES
    QElapsedTimer timer;
    timer.start();
#endif

#ifndef FONTA_DETAILED_DEBUG
    int cores = std::thread::hardware_concurrency();
    if(!cores) cores = 4;
    const int chunkN = out.size() / cores;

    std::vector<std::thread> futurs;

    int from = 0;
    int to = std::min(chunkN, out.size()-1);
    for(int i = 0; i<cores; ++i) {
        futurs.push_back( std::thread(loadTTFChunk, std::ref(out), from, to, std::ref(TTFs), std::ref(File2Fonts)) );
        from = to+1;

        if(i+1 >= (cores-1)) {
            to = out.size()-1;
        } else {
            to += chunkN;
        }
    }

    for(auto& f : futurs) {
        f.join();
    }
#else
    for(int i = 0; i<out.size(); ++i) {
        readFontFile(out[i], TTFs, File2Fonts);
    }
#endif

    // analyse fonts on common files
    for(cauto fontName : TTFs.keys()) {
        auto &TTF = TTFs[fontName];
        for(cauto f : TTF.files) {
            TTF.linkedFonts.unite(File2Fonts[f]);
        }
        TTF.linkedFonts.remove(fontName); // remove itself
    }

#ifdef FONTA_MEASURES
    qDebug() << timer.elapsed() << "milliseconds to load fonts";
    qDebug() << TTFs.size() << "fonts loaded";
#endif
}

DB::~DB()
{
    delete QtDB;
}

QStringList DB::families() const
{
    QStringList fonts = QtDB->families();
    QStringList uninstalledList = uninstalled();
    for(cauto f : uninstalledList) {
        fonts.removeAll(f);
    }

    return fonts;
}

QStringList DB::linkedFonts(CStringRef family) const
{
    TTF ttf;
    if(!getTTF(family, ttf)) {
        return QStringList();
    }

    return ttf.linkedFonts.toList();
}

QStringList DB::fontFiles(CStringRef family) const
{
    TTF ttf;
    if(!getTTF(family, ttf)) {
        return QStringList();
    }

    return ttf.files.toList();
}

void DB::uninstall(CStringRef family)
{
    /*
     * To uninstall font:
     *
     * 1. Remove appropriate record in HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Fonts registry
     * 2. Write uninstalled fonts to HKEY_LOCAL_MACHINE\FontaUninstalledFonts registry.
     *    Fonts listed here should not appear in Fonta's fonts list. After reboot fonts'll be removed completely
     *    and HKEY_LOCAL_MACHINE\FontaUninstalledFonts field'll be deleted automatically. (as every field at HKEY_LOCAL_MACHINE's root)
     * 3. Write uninstalled fonts' file paths at PitM\Fonta\FilesToDelete user's registry.
     *    This info'll be used at program startup when files are avaliable for deleting (see FontaDB constructor).
     */

    QSettings fontsReg("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", QSettings::NativeFormat);
    cauto files = fontaDB().fontFiles(family); // "C:/Windows/Fonts/arial.ttf"

    for(CStringRef f : files) {
        cauto regKeys = fontsReg.allKeys(); // "Arial (TrueType)"="arial.ttf"

        QFileInfo info(f);
        const QString name = info.fileName();

        for(CStringRef key : regKeys) {
            const QString value = fontsReg.value(key).toString();
            if(QString::compare(name, value, Qt::CaseInsensitive) == 0) {
                fontsReg.remove(key);
            }
        }
    }


    QStringList uninstalledList = uninstalled();
    uninstalledList << family;
    uninstalledList << linkedFonts(family);
    uninstalledList.removeDuplicates();

    QSettings uninstalledReg("HKEY_LOCAL_MACHINE", QSettings::NativeFormat);
    uninstalledReg.setValue("FontaUninstalledFonts", uninstalledList);

    QStringList filesToDeleteList = filesToDelete();
    filesToDeleteList << files;
    filesToDeleteList.removeDuplicates();

    QSettings fontaReg("PitM", "Fonta");
    fontaReg.setValue("FilesToDelete", filesToDeleteList);
}

QStringList DB::uninstalled() const
{
    QSettings uninstalledReg("HKEY_LOCAL_MACHINE", QSettings::NativeFormat);
    return uninstalledReg.value("FontaUninstalledFonts", QStringList()).toStringList();
}

QStringList DB::filesToDelete() const
{
    QSettings fontaReg("PitM", "Fonta");
    return fontaReg.value("FilesToDelete", QStringList()).toStringList();
}

bool DB::getTTF(CStringRef family, TTF& ttf) const {
    if(!TTFs.contains(family))
        return false;

    ttf = TTFs[family];
    return true;
}

FullFontInfo DB::getFullFontInfo(CStringRef family) const
{
    FullFontInfo fullInfo;

    TTF ttf;
    fullInfo.TTFExists = getTTF(family, ttf);
    fullInfo.fontaTFF = ttf;

    fullInfo.qtInfo.cyrillic = QtDB->writingSystems(family).contains(QFontDatabase::Cyrillic);
    fullInfo.qtInfo.symbolic = QtDB->writingSystems(family).contains(QFontDatabase::Symbol);
    fullInfo.qtInfo.monospaced = QtDB->isFixedPitch(family);

    return fullInfo;
}

static bool _isSerif(const TTF& ttf)
{
    if(FamilyClass::isSerif(ttf.familyClass)) {
        return true;
    }

    if(!FamilyClass::noInfo(ttf.familyClass)) {
        return false;
    }

    // 2
    return ttf.panose.isSerif();

    // 3 TODO: font name
}

bool DB::isSerif(CStringRef family) const
{
    // 1
    TTF ttf;
    if(!getTTF(family, ttf)) return false;

    return _isSerif(ttf);
}

static bool _isSansSerif(const TTF& ttf)
{
    if(FamilyClass::isSans(ttf.familyClass)) {
        return true;
    }

    if(!FamilyClass::noInfo(ttf.familyClass)) {
        return false;
    }

    // 2
    return ttf.panose.isSans();

    // 3 TODO: font name
}

bool DB::isSansSerif(CStringRef family) const
{
    // 1
    TTF ttf;
    if(!getTTF(family, ttf)) return false;

    return _isSansSerif(ttf);
}

bool DB::isMonospaced(CStringRef family) const
{
    bool qt = false;
    bool panose = false;

    qt = QtDB->isFixedPitch(family);

    TTF ttf;
    if(getTTF(family, ttf)) {
        panose = ttf.panose.isMonospaced();
    }

    return qt || panose;
}

bool DB::isScript(CStringRef family) const
{
    // 1
    TTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(ttf.familyClass == FamilyClass::SCRIPT) {
        return true;
    }

    if(!FamilyClass::noInfo(ttf.familyClass)) {
        return false;
    }

    // 2
    return ttf.panose.Family == Panose::FamilyType::SCRIPT;
}

bool DB::isDecorative(CStringRef family) const
{
    // 1
    TTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(ttf.familyClass == FamilyClass::ORNAMENTAL) {
        return true;
    }

    if(!FamilyClass::noInfo(ttf.familyClass)) {
        return false;
    }

    // 2
    return ttf.panose.Family == Panose::FamilyType::DECORATIVE;
}

bool DB::isSymbolic(CStringRef family) const
{
    // 1
    /*if(QtDB.writingSystems(family).contains(QFontDatabase::Symbol))
        return true;*/

    // 2
    TTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(ttf.familyClass == FamilyClass::SYMBOL) {
        return true;
    }

    if(!FamilyClass::noInfo(ttf.familyClass)) {
        return false;
    }

    // 3
    return ttf.panose.Family == Panose::FamilyType::SYMBOL;
}



bool DB::isOldStyle(CStringRef family) const
{
    TTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(ttf.familyClass != FamilyClass::OLDSTYLE_SERIF) return false;

    // 5 6 7 are Transitions
    return ttf.familySubClass != 5 && ttf.familySubClass != 6 && ttf.familySubClass != 7;
}

bool DB::isTransitional(CStringRef family) const
{
    TTF ttf;
    if(!getTTF(family, ttf)) return false;

    return ttf.familyClass == FamilyClass::TRANSITIONAL_SERIF
       || (ttf.familyClass == FamilyClass::CLARENDON_SERIF && (ttf.familySubClass == 2 || ttf.familySubClass == 3 || ttf.familySubClass == 4))
       || (ttf.familyClass == FamilyClass::OLDSTYLE_SERIF && (ttf.familySubClass == 5 || ttf.familySubClass == 6 || ttf.familySubClass == 7))
       ||  ttf.familyClass == FamilyClass::FREEFORM_SERIF;
}

bool DB::isModern(CStringRef family) const
{
    TTF ttf;
    if(!getTTF(family, ttf)) return false;

    return ttf.familyClass == FamilyClass::MODERN_SERIF;
}

bool DB::isSlab(CStringRef family) const
{
    TTF ttf;
    if(!getTTF(family, ttf)) return false;

    return ttf.familyClass == FamilyClass::SLAB_SERIF
       || (ttf.familyClass == FamilyClass::CLARENDON_SERIF && (ttf.familySubClass != 2 && ttf.familySubClass != 3 && ttf.familySubClass != 4));
}

bool DB::isCoveSerif(CStringRef family) const
{
    TTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(!_isSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle >= Panose::SerifStyle::COVE
        && ttf.panose.SerifStyle <= Panose::SerifStyle::OBTUSE_SQUARE_COVE;
}

bool DB::isSquareSerif(CStringRef family) const
{
    TTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(!_isSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::SQUARE
        || ttf.panose.SerifStyle == Panose::SerifStyle::THIN;
}

bool DB::isBoneSerif(CStringRef family) const
{
    TTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(!_isSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::OVAL;
}

bool DB::isAsymmetricSerif(CStringRef family) const
{
    TTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(!_isSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::ASYMMETRICAL;
}

bool DB::isTriangleSerif(CStringRef family) const
{
    TTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(!_isSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::TRIANGLE;
}


bool DB::isGrotesque(CStringRef family) const
{
    TTF ttf;
    if(!getTTF(family, ttf)) return false;

    return ttf.familyClass == FamilyClass::SANS_SERIF &&
            (ttf.familySubClass == 1
          || ttf.familySubClass == 5
          || ttf.familySubClass == 6
          || ttf.familySubClass == 9
          || ttf.familySubClass == 10);
}

bool DB::isGeometric(CStringRef family) const
{
    TTF ttf;
    if(!getTTF(family, ttf)) return false;

    return ttf.familyClass == FamilyClass::SANS_SERIF &&
            (ttf.familySubClass == 3
          || ttf.familySubClass == 4);
}

bool DB::isHumanist(CStringRef family) const
{
    TTF ttf;
    if(!getTTF(family, ttf)) return false;

    return ttf.familyClass == FamilyClass::SANS_SERIF &&
            (ttf.familySubClass == 2);
}


bool DB::isNormalSans(CStringRef family) const
{
    TTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(!_isSansSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::NORMAL_SANS
        || ttf.panose.SerifStyle == Panose::SerifStyle::OBTUSE_SANS
        || ttf.panose.SerifStyle == Panose::SerifStyle::PERPENDICULAR_SANS;
}

bool DB::isRoundedSans(CStringRef family) const
{
    TTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(!_isSansSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::ROUNDED;
}

bool DB::isFlarredSans(CStringRef family) const
{
    TTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(!_isSansSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::FLARED;
}

bool DB::isNonCyrillic(CStringRef family) const
{
    return !isCyrillic(family);
}

bool DB::isCyrillic(CStringRef family) const
{
    // 1
    if(QtDB->writingSystems(family).contains(QFontDatabase::Cyrillic))
        return true;

    // 2
    TTF ttf;
    if(!getTTF(family, ttf)) return false;

    return ttf.cyrillic;
}

bool DB::isNotLatinOrCyrillic(CStringRef family) const
{
    cauto systems = QtDB->writingSystems(family);
    if(systems.contains(QFontDatabase::Cyrillic)
    || systems.contains(QFontDatabase::Latin)
    || isSymbolic(family)) {
        return false;
    }

    TTF ttf;
    if(!getTTF(family, ttf)) return false;

    return !ttf.latin && !ttf.cyrillic;
}

} // namespace fonta
