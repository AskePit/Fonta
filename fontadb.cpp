#include "FontaDB.h"

#include <QDirIterator>
#include <QTextCodec>
#include <thread>
#include <QMutex>
#include <QElapsedTimer>
#include <QDebug>
#include <QStandardPaths>
#include <mutex>

static void GetFontFiles(QStringList &out)
{
    const auto fontsDirs = QStandardPaths::standardLocations(QStandardPaths::FontsLocation);
    for(const auto &dir : fontsDirs) {
        QDirIterator it(dir, QStringList() << "*.ttf" << "*.otf" << "*.ttc" << "*.otc" << "*.fon" , QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            it.next();
            out << it.filePath();
        }
    }
}

static inline u16 swapU16(u16 x) {
    return (x<<8) + (x>>8);
}

static inline u32 swapU32(u32 x) {
    u16 lo = (u16)x;
    u16 hi = x>>16;
    lo = swapU16(lo);
    hi = swapU16(hi);

    return (lo<<16) + hi;
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
};

struct TTFPostHeader {
    u32 Version;
    u32 ItalicAngle;
    u32 Underline;
    u32 isFixedPitch;
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
        POST,
        count
    };
}

// Forwards
static QString decodeFontName(u16 platformID, u16 encodingID, const QByteArray &nameBytes);
static void readFont(TTFOffsetTable tablesMap[], QFile &f, QHash<QString, FontaTTF> &TTFs);
static void readTTF(CStringRef fileName, QHash<QString, FontaTTF> &TTFs);
static void readTTC(CStringRef fileName, QHash<QString, FontaTTF> &TTFs);
static void readFON(CStringRef fileName, QHash<QString, FontaTTF> &TTFs);
static void readFontFile(CStringRef fileName, QHash<QString, FontaTTF> &TTFs);
static bool readTablesMap(TTFOffsetTable tablesMap[], QFile &f);

static std::mutex readTTFMutex;

static void readFontFile(CStringRef fileName, QHash<QString, FontaTTF> &TTFs)
{
    std::function<void(CStringRef fileName, QHash<QString, FontaTTF> &TTFs)> f;

    if(fileName.endsWith(".ttc", Qt::CaseInsensitive)) {
        f = readTTC;
    } else if(fileName.endsWith(".fon", Qt::CaseInsensitive)) {
        f = readFON;
    } else {
        f = readTTF;
    }

    f(fileName, TTFs);
}

static void readTTC(CStringRef fileName, QHash<QString, FontaTTF> &TTFs)
{
    qDebug() << qPrintable(QFileInfo(fileName).fileName()) << ":";
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open!";
        return;
    }

    f.seek(8);

    u32 offsetTablesCount;
    f.read((char*)&offsetTablesCount, 4);
    offsetTablesCount = swapU32(offsetTablesCount);

    std::vector<u32> offsets(offsetTablesCount);
    for(u32 i = 0; i<offsetTablesCount; ++i) {
        u32 &offset = offsets[i];
        f.read((char*)&offset, 4);
        offset = swapU32(offset);
    }

    for(u32 i = 0; i<offsetTablesCount; ++i) {
        f.seek(offsets[i] + 4);

        u16 fontTablesCount;
        f.read((char*)&fontTablesCount, 2);
        fontTablesCount = swapU16(fontTablesCount);

        f.seek(offsets[i] + 12);
        TTFOffsetTable tablesMap[TTFTable::count];
        int tablesCount = 0;
        for(int j = 0; j<fontTablesCount; ++j) {
            tablesCount += (int)readTablesMap(tablesMap, f);
            if(tablesCount == TTFTable::count) {
                break;
            }
        }

        if(tablesCount != TTFTable::count) {
            qWarning() << "no necessary tables!";
            return;
        }

        readFont(tablesMap, f, TTFs);
    }
}

static void readFON(CStringRef fileName, QHash<QString, FontaTTF> &TTFs)
{
    qDebug() << qPrintable(QFileInfo(fileName).fileName()) << ":";

    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open!";
        return;
    }

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
        fontName = s.trimmed();
    } else if(icomma != -1) {
        for(int i = icomma-1; i>=0; --i) {
            if(!s[i].isDigit()) {
                s.truncate(i+1);
                fontName = s.trimmed();
                break;
            }
        }
    } else {
        fontName = s.trimmed();
    }

    qDebug() << '\t' << fontName;

    if(TTFs.contains(fontName)) {
        TTFs[fontName].files << fileName;
        return;
    }

    FontaTTF ttf;
    ttf.files << fileName;

    std::lock_guard<std::mutex> lock(readTTFMutex);
    TTFs[fontName] = ttf;
    (void)lock;
}

static void readTTF(CStringRef fileName, QHash<QString, FontaTTF> &TTFs)
{
    qDebug() << qPrintable(QFileInfo(fileName).fileName()) << ":";

    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open!";
        return;
    }

    f.seek(12);

    TTFOffsetTable tablesMap[TTFTable::count];

    int tablesCount = 0;
    while(!f.atEnd()) {
        tablesCount += (int)readTablesMap(tablesMap, f);
        if(tablesCount == TTFTable::count) {
            break;
        }
    }

    if(tablesCount != TTFTable::count) {
        qWarning() << "no necessary tables!";
        return;
    }

    readFont(tablesMap, f, TTFs);
}

static bool readTablesMap(TTFOffsetTable tablesMap[], QFile &f)
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

    if(memcmp(offsetTable.TableName, "post", 4) == 0) {
        tableType = TTFTable::POST;
    }

    if(tableType != TTFTable::NO) {
        offsetTable.Length = swapU32(offsetTable.Length);
        offsetTable.Offset = swapU32(offsetTable.Offset);
        tablesMap[tableType] = offsetTable;
        return true;
    } else {
        return false;
    }
}

static void readFont(TTFOffsetTable tablesMap[], QFile &f, QHash<QString, FontaTTF> &TTFs)
{
    /////////
    // name
    ///////
    TTFOffsetTable &nameOffsetTable = tablesMap[TTFTable::NAME];
    f.seek(nameOffsetTable.Offset);

    TTFNameHeader nameHeader;
    f.read((char*)&nameHeader, sizeof(TTFNameHeader));
    nameHeader.RecordsCount = swapU16(nameHeader.RecordsCount);
    nameHeader.StorageOffset = swapU16(nameHeader.StorageOffset);

    TTFNameRecord nameRecord;
    QByteArray nameBytes;
    QString fontName;
    bool properLanguage = false;

    for(int i = 0; i<nameHeader.RecordsCount; ++i) {
        f.read((char*)&nameRecord, sizeof(TTFNameRecord));
        nameRecord.NameID = swapU16(nameRecord.NameID);

        // 1 says that this is font name. 2 says that this is font subname. 0 for example determines copyright info
        if(nameRecord.NameID == 1) {
            nameRecord.PlatformID = swapU16(nameRecord.PlatformID);
            nameRecord.EncodingID = swapU16(nameRecord.EncodingID);
            nameRecord.StringLength = swapU16(nameRecord.StringLength);
            nameRecord.StringOffset = swapU16(nameRecord.StringOffset);

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
                qDebug() << '\t' << nameRecord.PlatformID << nameRecord.EncodingID << langCode << fontName;
                break;
            }

            f.seek(nPos);
        }
    }

    if(!properLanguage) {
        // use last record to extract font name
        fontName = decodeFontName(nameRecord.PlatformID, nameRecord.EncodingID, nameBytes);
        qDebug() << '\t' << "not proper!" << nameRecord.PlatformID << nameRecord.EncodingID << (nameRecord.LanguageID>>8) << fontName;
    }

    if(TTFs.contains(fontName)) {
        TTFs[fontName].files << f.fileName();
        return;
    }

    FontaTTF ttf;
    ttf.files << f.fileName();
    //qDebug() << '\t' << fontName;

    /////////
    // post
    ///////
    TTFOffsetTable& postOffsetTable = tablesMap[TTFTable::POST];
    f.seek(postOffsetTable.Offset);

    TTFPostHeader postHeader;
    f.read((char*)&postHeader, sizeof(TTFPostHeader));

    ttf.monospaced = !!postHeader.isFixedPitch;

    /////////
    // OS/2
    ///////
    TTFOffsetTable& os2OffsetTable = tablesMap[TTFTable::OS2];
    f.seek(os2OffsetTable.Offset);

    TTFOS2Header os2Header;
    f.read((char*)&os2Header, sizeof(TTFOS2Header));
    os2Header.FamilyClass = swapU16(os2Header.FamilyClass);
    os2Header.UnicodeRange1 = swapU32(os2Header.UnicodeRange1);

    ttf.panose = os2Header.panose;
    ttf.familyClass = (FamilyClass::type)(os2Header.FamilyClass >> 8);
    ttf.familySubClass = (int)(os2Header.FamilyClass & 0xFF);
    ttf.cyrillic = !!(os2Header.UnicodeRange1 & (1<<9));

    std::lock_guard<std::mutex> lock(readTTFMutex);
    TTFs[fontName] = ttf;
    (void)lock;
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

static void loadTTFChunk(const QStringList &out, int from, int to, QHash<QString, FontaTTF> &TTFs)
{
    for(int i = from; i<=to; ++i) {
        readFontFile(out[i], TTFs);
    }
}

FontaDB::FontaDB()
{
    QStringList out;
    GetFontFiles(out);

    QElapsedTimer timer;
    timer.start();

    /*
    int cores = std::thread::hardware_concurrency();
    if(!cores) cores = 4;
    const int chunkN = out.size() / cores;

    std::vector<std::thread> futurs;

    int from = 0;
    int to = chunkN;
    for(int i = 0; i<cores; ++i) {
        futurs.push_back( std::thread(loadTTFChunk, out, from, to, std::ref(TTFs)) );
        from = to+1;
        to = std::min(to*chunkN, out.size()-1);
    }

    for(auto& f : futurs) {
        f.join();
    }
*/

    for(int i = 0; i<out.size(); ++i) {
        readFontFile(out[i], TTFs);
    }

    qDebug() << timer.elapsed() << "milliseconds to load fonts";
    qDebug() << TTFs.size() << "fonts loaded";
}

FontaDB::~FontaDB()
{}

bool FontaDB::getTTF(CStringRef family, FontaTTF& ttf) const {
    if(!TTFs.contains(family))
        return false;

    ttf = TTFs[family];
    return true;
}

FullFontInfo FontaDB::getFullFontInfo(CStringRef family) const
{
    FullFontInfo fullInfo;

    FontaTTF ttf;
    fullInfo.TTFExists = getTTF(family, ttf);
    fullInfo.fontaTFF = ttf;

    fullInfo.qtInfo.cyrillic = QtDB.writingSystems(family).contains(QFontDatabase::Cyrillic);
    fullInfo.qtInfo.symbolic = QtDB.writingSystems(family).contains(QFontDatabase::Symbol);
    fullInfo.qtInfo.monospaced = QtDB.isFixedPitch(family);

    return fullInfo;
}

bool FontaDB::isCyrillic(CStringRef family) const
{
    // 1
    if(QtDB.writingSystems(family).contains(QFontDatabase::Cyrillic))
        return true;

    // 2
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    return ttf.cyrillic;
}

static bool _isSerif(const FontaTTF& ttf)
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

bool FontaDB::isSerif(CStringRef family) const
{
    // 1
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    return _isSerif(ttf);
}

static bool _isSansSerif(const FontaTTF& ttf)
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

bool FontaDB::isSansSerif(CStringRef family) const
{
    // 1
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    return _isSansSerif(ttf);
}

bool FontaDB::isMonospaced(CStringRef family) const
{
    // 1
    if(QtDB.isFixedPitch(family))
        return true;

    // 2
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(ttf.panose.isMonospaced())
        return true;

    // 3
    return ttf.monospaced;
}

bool FontaDB::isScript(CStringRef family) const
{
    // 1
    FontaTTF ttf;
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

bool FontaDB::isDecorative(CStringRef family) const
{
    // 1
    FontaTTF ttf;
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

bool FontaDB::isSymbolic(CStringRef family) const
{
    // 1
    /*if(QtDB.writingSystems(family).contains(QFontDatabase::Symbol))
        return true;*/

    // 2
    FontaTTF ttf;
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



bool FontaDB::isOldStyle(CStringRef family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(ttf.familyClass != FamilyClass::OLDSTYLE_SERIF) return false;

    // 5 6 7 are Transitions
    return ttf.familySubClass != 5 && ttf.familySubClass != 6 && ttf.familySubClass != 7;
}

bool FontaDB::isTransitional(CStringRef family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    return ttf.familyClass == FamilyClass::TRANSITIONAL_SERIF
       || (ttf.familyClass == FamilyClass::CLARENDON_SERIF && (ttf.familySubClass == 2 || ttf.familySubClass == 3 || ttf.familySubClass == 4))
       || (ttf.familyClass == FamilyClass::OLDSTYLE_SERIF && (ttf.familySubClass == 5 || ttf.familySubClass == 6 || ttf.familySubClass == 7))
       ||  ttf.familyClass == FamilyClass::FREEFORM_SERIF;
}

bool FontaDB::isModern(CStringRef family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    return ttf.familyClass == FamilyClass::MODERN_SERIF;
}

bool FontaDB::isSlab(CStringRef family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    return ttf.familyClass == FamilyClass::SLAB_SERIF
       || (ttf.familyClass == FamilyClass::CLARENDON_SERIF && (ttf.familySubClass != 2 && ttf.familySubClass != 3 && ttf.familySubClass != 4));
}

bool FontaDB::isCoveSerif(CStringRef family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(!_isSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle >= Panose::SerifStyle::COVE
        && ttf.panose.SerifStyle <= Panose::SerifStyle::OBTUSE_SQUARE_COVE;
}

bool FontaDB::isSquareSerif(CStringRef family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(!_isSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::SQUARE
        || ttf.panose.SerifStyle == Panose::SerifStyle::THIN;
}

bool FontaDB::isBoneSerif(CStringRef family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(!_isSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::OVAL;
}

bool FontaDB::isAsymmetricSerif(CStringRef family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(!_isSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::ASYMMETRICAL;
}

bool FontaDB::isTriangleSerif(CStringRef family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(!_isSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::TRIANGLE;
}


bool FontaDB::isGrotesque(CStringRef family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    return ttf.familyClass == FamilyClass::SANS_SERIF &&
            (ttf.familySubClass == 1
          || ttf.familySubClass == 5
          || ttf.familySubClass == 6
          || ttf.familySubClass == 9
          || ttf.familySubClass == 10);
}

bool FontaDB::isGeometric(CStringRef family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    return ttf.familyClass == FamilyClass::SANS_SERIF &&
            (ttf.familySubClass == 3
          || ttf.familySubClass == 4);
}

bool FontaDB::isHumanist(CStringRef family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    return ttf.familyClass == FamilyClass::SANS_SERIF &&
            (ttf.familySubClass == 2);
}


bool FontaDB::isNormalSans(CStringRef family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(!_isSansSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::NORMAL_SANS
        || ttf.panose.SerifStyle == Panose::SerifStyle::OBTUSE_SANS
        || ttf.panose.SerifStyle == Panose::SerifStyle::PERPENDICULAR_SANS;
}

bool FontaDB::isRoundedSans(CStringRef family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(!_isSansSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::ROUNDED;
}

bool FontaDB::isFlarredSans(CStringRef family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(!_isSansSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::FLARED;
}

